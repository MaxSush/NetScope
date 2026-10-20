#include "PersistenceService.h"

#include <unordered_map>
#include <logger/Logger.h>

#include <config/Config.h>

namespace netscope::storage
{
	netscope::storage::PersistenceService::PersistenceService(IDatabase& database, const hostname::DnsCache& dnsCache, hostname::RdnsResolver& rdnsResolver)
		: m_processRepository(database),
		m_flowRepository(database),
		m_bandwidthRepository(database),
		m_appStateRepository(database),
		m_interfaceBandwidthRepository(database),
		m_db(database),
		m_dnsCache(dnsCache),
		m_rdnsResolver(rdnsResolver)
	{
		m_batchBuffer.reserve(Config::Get().persistenceBatchSize);
	}

	void PersistenceService::Run()
	{
		try
		{
			BuildBatch();

			Optimize();

			auto res = PersistBatch();
			if (!res)
			{
				LOG_ERROR("PersistBatch() failed: " + res.message);
			}
		}
		catch (const std::runtime_error&)
		{
			return;
		}
	}

	void PersistenceService::Stop()
	{
		m_queue.Shutdown();
	}

	void PersistenceService::RunRetention()
	{
		const auto& config = Config::Get();
		auto now = ToUnixMilliseconds(System_Clock::now());

		auto res = m_db.BeginTransaction();
		if (!res)
		{
			LOG_ERROR("Retention transaction failed : {}", res.message);
			return;
		}

		// 1. Purge old closed flows
		if (config.retentionHours > 0)
		{
			int64_t flowCutoff = now - (static_cast<int64_t>(config.retentionHours) * 3600000);

			constexpr auto PurgeFlowSql = R"sql(
				DELETE FROM flows WHERE state = 'CLOSED' AND last_seen < ?;
			)sql";
			
			std::unique_ptr<Statement> statement;
			res = m_db.Prepare(PurgeFlowSql, statement);
			if (res)
			{
				statement->Bind(1, flowCutoff);
				statement->Execute();
			}
		}

		// 2. Roll up yesterday's hourly data into daily
		int64_t todayStart = (now / 86400000) * 86400000;
		int64_t yesterdayStart = todayStart - 86400000;
		m_bandwidthRepository.RollUpDaily(yesterdayStart);

		// 3. Purge Bandwidth data
		int64_t hourlyCutoff = now - (static_cast<int64_t>(config.hourlyRetentionDays) * 86400000);
		m_bandwidthRepository.PurgeHourlyBefore(hourlyCutoff);

		int64_t dailyCutoff = now - (static_cast<int64_t>(config.dailyRetentionDays) * 86400000);
		m_bandwidthRepository.PurgeDailyBefore(dailyCutoff);

		// 4. Purge orphaned processes
		constexpr auto PurgeOrphanSql = R"sql(
			DELETE FROM processes
			WHERE process_id NOT IN (SELECT DISTINCT process_id FROM flows)
			  AND end_time IS NOT NULL;
		)sql";
		m_db.Execute(PurgeOrphanSql);

		// 5. Roll up interface bandwidth
		m_interfaceBandwidthRepository.RollUpDaily(yesterdayStart);

		// 6. Purge interface bandwidth data
		m_interfaceBandwidthRepository.PurgeHourlyBefore(hourlyCutoff);
		m_interfaceBandwidthRepository.PurgeDailyBefore(dailyCutoff);

		res = m_db.Commit();
		if (!res)
		{
			LOG_ERROR("Retention commit failed: {}", res.message);
			m_db.Rollback();
		}
		else
		{
			LOG_INFO("Retention policy executed successfully");
		}
	}
	
	void PersistenceService::AccumulateBandwidth(int64_t processId, const flow::FlowSnapshot& snapshot, const flow::FlowStatistics& delta)
	{
		if (delta.uploadBytes == 0 && delta.downloadBytes == 0)
		{
			return;
		}

		auto now = ToUnixMilliseconds(snapshot.lastSeen);
		int64_t hourStart = (now / 3600000) * 3600000;

		auto res = m_bandwidthRepository.AccumulateHourly(processId, hourStart, 
			delta.uploadBytes, delta.downloadBytes, 1);

		if (!res)
		{
			LOG_WARN("Failed to accumulate bandwidth: {}", res.message);
		}
	}

	int64_t PersistenceService::ResolveProcessId(const process::ProcessInfo& process)
	{
		auto it = m_processIdCache.find(process.pid);
		if (it != m_processIdCache.end())
		{
			return it->second;
		}
		auto result = m_processRepository.GetProcessId(process.pid, process.start_time);
		if (result.HasValue())
		{
			m_processIdCache[process.pid] = result.Value();
			return result.Value();
		}
		
		LOG_ERROR("Failed to resolve process_id for pid={}", process.pid);
		return -1;
	}

	int64_t PersistenceService::ResolveProcessId(uint32_t pid)
	{
		auto it = m_processIdCache.find(pid);
		if (it != m_processIdCache.end())
		{
			return it->second;
		}

		// Flow update without a cached process_id means we missed the FlowCreated.
		LOG_WARN("process_id cache miss for pid={}", pid);
		return -1;
	}

	void PersistenceService::BuildBatch()
	{
		m_batchBuffer.clear();

		m_batchBuffer.push_back(m_queue.Pop());

		auto deadline = std::chrono::steady_clock::now() + Config::Get().persistenceBatchDelay;

		while (m_batchBuffer.size() < Config::Get().persistenceBatchSize)
		{
			auto event = m_queue.TryPop();
			if (event)
			{
				m_batchBuffer.push_back(std::move(*event));
				continue;
			}

			auto now = Steady_Clock::now();
			if (now >= deadline)
			{
				break;
			}

			auto remaining = std::chrono::duration_cast<Milliseconds>(deadline - now);

			auto popped = m_queue.PopFor(remaining);
			if (!popped.has_value())
			{
				break;
			}
			m_batchBuffer.push_back(std::move(*popped));
		}
	}

	void PersistenceService::Optimize()
	{
		// optimization
		std::unordered_map<flow::FlowKey, FlowBatchEntry> flowMap;

		std::vector<PersistenceEvent> optimized;
		optimized.reserve(m_batchBuffer.size());

		for (auto& event : m_batchBuffer)
		{
			std::visit(
				[&](const auto& e) {
					using T = std::decay_t<decltype(e)>;
					if constexpr (std::is_same_v<T, processing::FlowUpdatedEvent>)
					{
						auto it = flowMap.find(e.flowSnapshot.key);

						if (it == flowMap.end())
						{
							flowMap.emplace(e.flowSnapshot.key, FlowBatchEntry{ optimized.size(), FlowBatchState::UPDATED });
							optimized.push_back(event);
							//LOG_INFO("Updated");
						}
						else
						{
							//LOG_INFO("Replacing");
							auto& entry = it->second;
							if (entry.state == FlowBatchState::CREATED)
							{
								auto& created = std::get<processing::FlowCreatedEvent>(optimized[entry.index]);
								created.flowSnapshot = e.flowSnapshot;
							}
							else
							{
								optimized[entry.index] = event;
							}
						}
					}
					else if constexpr (std::is_same_v<T, processing::FlowCreatedEvent>)
					{
						//LOG_INFO("Created");
						flowMap.emplace(e.flowSnapshot.key, FlowBatchEntry{ optimized.size(), FlowBatchState::CREATED });
						optimized.push_back(event);

					}
					else if constexpr (std::is_same_v<T, processing::FlowClosedEvent>)
					{
						//LOG_INFO("Closed");
						auto it = flowMap.find(e.flowSnapshot.key);
						if (it == flowMap.end())
						{
							flowMap.emplace(e.flowSnapshot.key, FlowBatchEntry{ optimized.size(), FlowBatchState::CLOSED });
							optimized.push_back(event);
						}
						else
						{
							auto& entry = it->second;
							if (entry.state == FlowBatchState::CREATED)
							{
								optimized.push_back(event);
								entry.index = optimized.size() - 1;
								entry.state = FlowBatchState::CLOSED;
							}
							else
							{
								optimized[entry.index] = event;
								entry.state = FlowBatchState::CLOSED;
							}
						}
					}
					else if constexpr (std::is_same_v<T, processing::ProcessClosedEvent>)
					{
						optimized.push_back(event);
					}
					else
					{
						LOG_ERROR("Event Dropped");
					}
				},
				event
			);
		}
		LOG_INFO("Input batch size: {}", m_batchBuffer.size());
		LOG_INFO("Optimized size: {}", optimized.size());

		m_batchBuffer.swap(optimized);
	}

	DatabaseResult PersistenceService::PersistBatch()
	{
		if (m_batchBuffer.empty())
		{
			return {};
		}

		auto res = m_db.BeginTransaction();
		if (!res)
		{
			return res;
		}

		for (const auto& event : m_batchBuffer)
		{
			std::visit([this, &res](const auto& e)
				{
					res = Persist(e);
				},
				event
			);
			if (!res)
			{
				m_db.Rollback();
				return res;
			}
		}

		return m_db.Commit();
	}

	void PersistenceService::HandleEvent(const processing::FlowCreatedEvent& event)
	{
		m_queue.Push(event);
	}

	void PersistenceService::HandleEvent(const processing::FlowUpdatedEvent& event)
	{
		m_queue.Push(event);
	}

	void PersistenceService::HandleEvent(const processing::FlowClosedEvent& event)
	{
		m_queue.Push(event);
	}

	void PersistenceService::HandleEvent(const processing::ProcessClosedEvent& event)
	{
		m_queue.Push(event);
	}

	DatabaseResult PersistenceService::Persist(const processing::FlowCreatedEvent& event)
	{
		auto res = m_processRepository.Upsert(event.process);
		if (!res)
		{
			return res;
		}

		int64_t processId = ResolveProcessId(event.process);
		if (processId == -1)
		{
			return DatabaseResult::Failure(
				DatabaseStatus::FAILED, DatabaseError::UNKNOWN,
				"Failed to resolve process_id"
			);
		}

		
		auto hostname = ResolveHostname(event.flowSnapshot.key.destination.address);
		auto interfaceName = ResolveInterfaceName(event.flowSnapshot);

		res =  m_flowRepository.Upsert(event.flowSnapshot, processId, hostname, interfaceName);
		if (!res)
		{
			return res;
		}

		const auto& stats = event.flowSnapshot.statistics;
		AccumulateBandwidth(processId, event.flowSnapshot, stats);

		if (interfaceName.has_value())
		{
			AccumulateInterfaceBandwidth(*interfaceName, event.flowSnapshot, stats);
		}

		m_lastPersistedStats[event.flowSnapshot.key] = stats;

		return DatabaseResult::Ok();

	}

	DatabaseResult PersistenceService::Persist(const processing::FlowUpdatedEvent& event)
	{
		int64_t processId = ResolveProcessId(event.flowSnapshot.pid);
		if (processId == -1)
		{
			return DatabaseResult::Failure(
				DatabaseStatus::FAILED, DatabaseError::UNKNOWN,
				"process_id not found in cache for flow update"
			);
		}

		auto hostname = ResolveHostname(event.flowSnapshot.key.destination.address);
		auto interfaceName = ResolveInterfaceName(event.flowSnapshot);

		auto res = m_flowRepository.Upsert(event.flowSnapshot, processId, hostname, interfaceName);
		if (!res)
		{
			return res;
		}

		// Compute delta
		const auto& stats = event.flowSnapshot.statistics;
		flow::FlowStatistics delta{};

		auto it = m_lastPersistedStats.find(event.flowSnapshot.key);
		if (it != m_lastPersistedStats.end())
		{
			const auto& prev = it->second;
			delta.uploadBytes = stats.uploadBytes - prev.uploadBytes;
			delta.downloadBytes = stats.downloadBytes - prev.downloadBytes;
			delta.uploadPackets = stats.uploadPackets - prev.uploadPackets;
			delta.downloadPackets = stats.downloadPackets - prev.downloadPackets;
		}
		else
		{
			delta = stats;
		}

		AccumulateBandwidth(processId, event.flowSnapshot, delta);

		if (interfaceName.has_value())
		{
			AccumulateInterfaceBandwidth(*interfaceName, event.flowSnapshot, delta);
		}

		m_lastPersistedStats[event.flowSnapshot.key] = stats;

		return DatabaseResult::Ok();
	}

	DatabaseResult PersistenceService::Persist(const processing::FlowClosedEvent& event)
	{
		int64_t processId = ResolveProcessId(event.flowSnapshot.pid);
		if (processId == -1)
		{
			return DatabaseResult::Failure(
				DatabaseStatus::FAILED, DatabaseError::UNKNOWN,
				"process_id not found in cache for flow close"
			);
		}

		auto hostname = ResolveHostname(event.flowSnapshot.key.destination.address);
		auto interfaceName = ResolveInterfaceName(event.flowSnapshot);

		auto res =  m_flowRepository.Upsert(event.flowSnapshot, processId, hostname, interfaceName);
		if (!res)
		{
			return res;
		}

		const auto& stats = event.flowSnapshot.statistics;
		flow::FlowStatistics delta{};

		auto it = m_lastPersistedStats.find(event.flowSnapshot.key);
		if (it != m_lastPersistedStats.end())
		{
			const auto& prev = it->second;
			delta.uploadBytes = stats.uploadBytes - prev.uploadBytes;
			delta.downloadBytes = stats.downloadBytes - prev.downloadBytes;
			delta.uploadPackets = stats.uploadPackets - prev.uploadPackets;
			delta.downloadPackets = stats.downloadPackets - prev.downloadPackets;
		}
		else
		{
			delta = stats;
		}

		AccumulateBandwidth(processId, event.flowSnapshot, delta);

		if (interfaceName.has_value())
		{
			AccumulateInterfaceBandwidth(*interfaceName, event.flowSnapshot, delta);
		}

		m_lastPersistedStats.erase(event.flowSnapshot.key);

		return DatabaseResult::Ok();
	}

	DatabaseResult netscope::storage::PersistenceService::Persist(const processing::ProcessClosedEvent& event)
	{
		auto res = m_processRepository.Upsert(event.processInfo);

		// evict from cache
		m_processIdCache.erase(event.processInfo.pid);

		return res;
	}
	std::optional<std::string> netscope::storage::PersistenceService::ResolveHostname(const flow::IPAddress& ip)
	{
		auto hostname = m_dnsCache.Lookup(ip);

		if (!hostname.has_value())
		{
			m_rdnsResolver.Enqueue(ip);
			return std::nullopt;
		}

		if (hostname->empty())
		{
			return std::nullopt;
		}

		return hostname;
	}

	void PersistenceService::AccumulateInterfaceBandwidth(const std::string& interfaceName, const flow::FlowSnapshot& snapshot, const flow::FlowStatistics& delta)
	{
		if (delta.uploadBytes == 0 && delta.downloadBytes == 0)
		{
			return;
		}

		auto now = ToUnixMilliseconds(snapshot.lastSeen);
		int64_t hourStart = (now / 3600000) * 3600000;

		auto res = m_interfaceBandwidthRepository.AccumulateHourly(interfaceName, hourStart, delta.uploadBytes, delta.downloadBytes, delta.uploadPackets, delta.downloadPackets);
		if (!res)
		{
			LOG_WARN("Failed to accumulate interface bandwidth: {}", res.message);
		}
	}

	std::optional<std::string> netscope::storage::PersistenceService::ResolveInterfaceName(const flow::FlowSnapshot& snapshot)
	{
		uint32_t ifindex = snapshot.ifindex;

		if (ifindex == 0)
		{
			ifindex = m_interfaceResolver.ResolveRoute(snapshot.destination.address);
		}

		if (ifindex == 0)
		{
			return std::nullopt;
		}

		auto name = m_interfaceResolver.Resolve(ifindex);
		if (name == "unknown")
		{
			return std::nullopt;
		}

		return name;
	}
}