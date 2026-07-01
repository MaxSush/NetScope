#include "ProcessCache.h"

namespace netscope
{
	ProcessCache::ProcessCache(ProcessManager& processManager)
		:
		m_processManager(processManager)
	{
	}

	std::optional<ProcessInfo> ProcessCache::FindProcess(uint32_t pid)
	{
		auto now = std::chrono::steady_clock::now();
		auto it = m_cache.find(pid);

		if (it != m_cache.end())
		{
			it->second.lastAccess = now;
			return it->second.process;
		}

		auto process = m_processManager.FindProcess(pid);
		if (!process)
		{
			return std::nullopt;
		}

		m_cache.emplace(pid, 
			CachedProcess
			{ 
				*process,
				now 
			}
		);
		return process;
	}

	void ProcessCache::CleanUp(std::chrono::seconds maxAge)
	{
		auto now = std::chrono::steady_clock::now();

		for (auto it = m_cache.begin(); it != m_cache.end();)
		{
			auto process = m_processManager.FindProcess(it->second.process.pid);
			if (!process)
			{
				it = m_cache.erase(it);
				continue;
			}

			if (it->second.process.start_time != process->start_time)
			{
				it = m_cache.erase(it);
				continue;
			}

			if (now - it->second.lastAccess > maxAge)
			{
				it = m_cache.erase(it);
				continue;
			}
			++it;
		}
	}
}
