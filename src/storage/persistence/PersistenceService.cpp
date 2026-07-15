#include "PersistenceService.h"


namespace netscope::storage
{
	netscope::storage::PersistenceService::PersistenceService(IDatabase& database)
		: m_processRepository(database),
		m_flowRepository(database),
		m_db(database)
	{
	}

	DatabaseResult PersistenceService::SaveFlow(const process::ProcessInfo& process, const flow::Flow& flow)
	{
		auto res = m_db.BeginTransaction();

		if (!res)
		{
			return res;
		}

		res = m_processRepository.Upsert(process);
		if (!res)
		{
			m_db.Rollback();
			return res;
		}

		res = m_flowRepository.Upsert(flow);
		if (!res)
		{
			m_db.Rollback();
			return res;
		}

		return m_db.Commit();
	}

	DatabaseResult PersistenceService::EndProcess(const process::ProcessInfo& process)
	{
		return DatabaseResult();
	}

	DatabaseResult PersistenceService::CloseFlow(const flow::Flow& flow)
	{
		return DatabaseResult();
	}

}