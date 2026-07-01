#pragma once

#include "ProcessManager.h"
#include "ProcessInfo.h"
#include "CachedProcess.h"

#include <chrono>
#include <unordered_map>
#include <optional>

namespace netscope
{
	class ProcessCache
	{
	public:
		explicit ProcessCache(ProcessManager& processManager);

		std::optional<ProcessInfo> FindProcess(uint32_t pid);
		void CleanUp(std::chrono::seconds maxAge);

	private:
		ProcessManager& m_processManager;

		std::unordered_map<uint32_t, CachedProcess> m_cache;
	};
}