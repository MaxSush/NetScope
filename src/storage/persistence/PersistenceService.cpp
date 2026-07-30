#include "PersistenceService.h"
#include "PersistenceService.h"
#include "PersistenceService.h"

#include <unordered_map>
#include <FlowHash.h>
#include <logger/Logger.h>

namespace netscope::storage
{
	netscope::storage::PersistenceService::PersistenceService(IDatabase& database)
		: m_processRepository(database),
		m_flowRepository(database),
		m_db(database)
	{
		m_batchBuffer.reserve(MAX_BATCH_SIZE);
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
				LOG_ERROR(res.message);
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
	
	void PersistenceService::BuildBatch()
	{
		m_batchBuffer.clear();

		m_batchBuffer.push_back(m_queue.Pop());

		auto deadline = std::chrono::steady_clock::now() + MAX_BATCH_DELAY;

		while (m_batchBuffer.size() < MAX_BATCH_SIZE)
		{
			PersistenceEvent event;
			if (m_queue.TryPop(event))
			{
				m_batchBuffer.push_back(std::move(event));
				continue;
			}

			auto now = Steady_Clock::now();
			if (now >= deadline)
			{
				break;
			}

			auto remaining = std::chrono::duration_cast<Milliseconds>(deadline - now);

			if (!m_queue.PopFor(event, remaining))
			{
				break;
			}
			m_batchBuffer.push_back(std::move(event));
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
			std::visit([this](const auto& e)
				{
					//LOG_INFO("Persistence Event");
					Persist(e);
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

	DatabaseResult PersistenceService::Persist(const processing::FlowCreatedEvent& event)
	{
		auto res = m_processRepository.Upsert(event.process);
		if (!res)
		{
			return res;
		}
		return m_flowRepository.Upsert(event.flowSnapshot);
	}

	DatabaseResult PersistenceService::Persist(const processing::FlowUpdatedEvent& event)
	{
		return m_flowRepository.Upsert(event.flowSnapshot);
	}

	DatabaseResult PersistenceService::Persist(const processing::FlowClosedEvent& event)
	{
		// TODO: Handle process cleanup if needed
		return m_flowRepository.Upsert(event.flowSnapshot);
	}


}