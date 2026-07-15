#pragma once
#include <database/IDatabase.h>
#include <repository/ProcessRepository.h>
#include <repository/FlowRepository.h>
#include <model/Flow.h>


namespace netscope::storage
{
	class PersistenceService
	{
	public:
		explicit PersistenceService(IDatabase& database);

		DatabaseResult SaveFlow(const ProcessInfo& process, const Flow& flow);

		DatabaseResult EndProcess(const ProcessInfo& process);

		DatabaseResult CloseFlow(const Flow& flow);

		//DatabaseResult SaveSnapshot(const Snapshot& snapshot);

	private:
		IDatabase& m_db;

		ProcessRepository m_processRepository;
		FlowRepository m_flowRepository;
		//SnapshotRepository m_snapshotRepository;


	};
}