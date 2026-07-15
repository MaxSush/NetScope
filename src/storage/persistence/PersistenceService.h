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

		DatabaseResult SaveFlow(const process::ProcessInfo& process, const flow::Flow& flow);

		DatabaseResult EndProcess(const process::ProcessInfo& process);

		DatabaseResult CloseFlow(const flow::Flow& flow);

		//DatabaseResult SaveSnapshot(const Snapshot& snapshot);

	private:
		IDatabase& m_db;

		ProcessRepository m_processRepository;
		FlowRepository m_flowRepository;
		//SnapshotRepository m_snapshotRepository;


	};
}