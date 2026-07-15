#pragma once

#include "IProcessRepository.h"
#include <database/IDatabase.h>

namespace netscope::storage
{
	class ProcessRepository : IProcessRepository
	{
	public:
		explicit ProcessRepository(IDatabase& database);

		// Inherited via IProcessRepository
		DatabaseResult Upsert(const process::ProcessInfo& process) override;

		Expected<process::ProcessInfo, DatabaseResult> Find(uint32_t pid, uint64_t startTime) override;

	private:
		IDatabase& m_db;
	};

}