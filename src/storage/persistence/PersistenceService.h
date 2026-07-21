#pragma once
#include <database/IDatabase.h>
#include <repository/ProcessRepository.h>
#include <repository/FlowRepository.h>
#include <event/EventQueue.h>

#include <event/IEventSubscriber.h>
#include <activity/FlowUpdatedEvent.h>
#include <activity/FlowCreatedEvent.h>

#include <thread>
#include <atomic>

namespace netscope::storage
{

	using PersistenceEvent = std::variant<
		processing::FlowCreatedEvent,
		processing::FlowUpdatedEvent
	>;

	class PersistenceService : 
		public core::IEventSubscriber<processing::FlowCreatedEvent>,
		public core::IEventSubscriber<processing::FlowUpdatedEvent>
	{
	public:
		explicit PersistenceService(IDatabase& database);

		void Run();
		void Stop();

		//DatabaseResult SaveSnapshot(const Snapshot& snapshot);

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


		static constexpr size_t MAX_BATCH_SIZE = 1000;
		static constexpr Milliseconds MAX_BATCH_DELAY{ 1000 };

		IDatabase& m_db;

		EventQueue<PersistenceEvent> m_queue;

		ProcessRepository m_processRepository;
		FlowRepository m_flowRepository;
		//SnapshotRepository m_snapshotRepository;

		std::vector<PersistenceEvent> m_batchBuffer;

		void BuildBatch();
		void Optimize();
		DatabaseResult PersistBatch();

		void HandleEvent(const processing::FlowCreatedEvent& event) override;
		void HandleEvent(const processing::FlowUpdatedEvent& event) override;

		DatabaseResult Persist(const processing::FlowCreatedEvent& event);
		DatabaseResult Persist(const processing::FlowUpdatedEvent& event);
	};
}