#pragma once
#include "database/DatabaseResult.h"
#include <utilities/Expected.h>
#include <ProcessInfo.h>
#include <cstdint>
#include <optional>

namespace netscope::storage
{
	class IProcessRepository
	{
	public:
		virtual ~IProcessRepository() = default;

		virtual DatabaseResult Upsert(const ProcessInfo& process) = 0;

		virtual Expected<ProcessInfo, DatabaseResult> Find(uint32_t pid, uint64_t startTime) = 0;
	};
}