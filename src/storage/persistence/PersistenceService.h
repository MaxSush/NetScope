#pragma once
#include <database/IDatabase.h>
#include <repository/ProcessRepository.h>
#include <repository/FlowRepository.h>
#include <repository/BandwidthRepository.h>
#include <repository/AppStateRepository.h>
#include <event/EventQueue.h>
#include <cache/DnsCache.h>
#include <resolver/RdnsResolver.h>
#include <repository/InterfaceBandwidthRepository.h>
#include <net/InterfaceResolver.h>

#include <FlowHash.h>

#include <event/IEventSubscriber.h>
#include <activity/FlowUpdatedEvent.h>
#include <activity/FlowCreatedEvent.h>
#include <activity/FlowClosedEvent.h>
#include <activity/ProcessClosedEvent.h>

#include <thread>
#include <atomic>
#include <unordered_map>

namespace netscope::storage
{

	using PersistenceEvent = std::variant<
		processing::FlowCreatedEvent,
		processing::FlowUpdatedEvent,
		processing::FlowClosedEvent,
		processing::ProcessClosedEvent
	>;

	class PersistenceService : 
		public core::IEventSubscriber<processing::FlowCreatedEvent>,
		public core::IEventSubscriber<processing::FlowUpdatedEvent>,
		public core::IEventSubscriber<processing::FlowClosedEvent>,
		public core::IEventSubscriber<processing::ProcessClosedEvent>
	{
	public:
		explicit PersistenceService(IDatabase& database, const hostname::DnsCache& dnsCache, hostname::RdnsResolver& rdnsResolver);

		void Run();
		void Stop();
		void RunRetention();

	private:
		enum class FlowBatchState
		{
			CREATED,
			UPDATED,
			CLOSED
		};

		struct FlowBatchEntry
		{
			std::size_t index;
			FlowBatchState state;
		};

		IDatabase& m_db;

		EventQueue<PersistenceEvent> m_queue;

		ProcessRepository m_processRepository;
		FlowRepository m_flowRepository;

		BandwidthRepository m_bandwidthRepository;
		AppStateRepository m_appStateRepository;

		std::unordered_map<flow::FlowKey, flow::FlowStatistics> m_lastPersistedStats;

		std::vector<PersistenceEvent> m_batchBuffer;
		std::unordered_map<uint32_t, int64_t> m_processIdCache;

		const hostname::DnsCache& m_dnsCache;
		hostname::RdnsResolver& m_rdnsResolver;

		InterfaceBandwidthRepository m_interfaceBandwidthRepository;
		InterfaceResolver m_interfaceResolver;

	private:
		void AccumulateBandwidth(int64_t processId, const flow::FlowSnapshot& snapshot, const flow::FlowStatistics& delta);

		int64_t ResolveProcessId(const process::ProcessInfo& process);
		int64_t ResolveProcessId(uint32_t pid);

		void BuildBatch();
		void Optimize();
		DatabaseResult PersistBatch();

		void HandleEvent(const processing::FlowCreatedEvent& event) override;
		void HandleEvent(const processing::FlowUpdatedEvent& event) override;
		void HandleEvent(const processing::FlowClosedEvent& event) override;
		void HandleEvent(const processing::ProcessClosedEvent& event) override;

		DatabaseResult Persist(const processing::FlowCreatedEvent& event);
		DatabaseResult Persist(const processing::FlowUpdatedEvent& event);
		DatabaseResult Persist(const processing::FlowClosedEvent& event);
		DatabaseResult Persist(const processing::ProcessClosedEvent& event);

		std::optional<std::string> ResolveHostname(const flow::IPAddress& ip);

		void AccumulateInterfaceBandwidth(const std::string& interfaceName, const flow::FlowSnapshot& snapshot, const flow::FlowStatistics& delta);
		std::optional<std::string> ResolveInterfaceName(const flow::FlowSnapshot& snapshot);
	};
}