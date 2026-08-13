#pragma once

#include "ProcessInfo.h"
#include <pipeline/IContextProcessor.h>
#include <lifecycle/ILifecycleProcessor.h>
#include <runtime/IBpfTracker.h>

#include <optional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>

#include <Config.h>


namespace netscope::process
{
	enum class ProcessState
	{
		TERMINATED,
		RUNNING,
		UNKNOWN
	};

	struct CachedProcess
	{
		ProcessInfo process;
		ProcessState state;
		std::chrono::steady_clock::time_point lastAccess;

		bool CanEvict(Steady_TimePoint now) const
		{
			return (now - lastAccess) >= config::PROCESS_EVICTION_TIMEOUT;
		}
	};

	class ProcessManager :	public processing::IContextProcessor,
							public processing::ILifecycleProcessor
	{
	public:
		explicit ProcessManager(IBpfTracker& tracker);

		void Process(processing::ProcessingContext& context) override;

		// Inherited via ILifecycleProcessor
		void Process(processing::LifecycleContext& context) override;
		void CleanUpExpired(Steady_TimePoint now) override;

		std::vector<ProcessInfo> GetProcesses() const;
		std::optional<ProcessResult> Resolve(uint32_t pid);

	private:
		IBpfTracker& m_tracker;

		bool IsPidDirectory(const std::string& name) const;
		std::optional<ProcessInfo> ReadProcess(const uint32_t pid) const;
		void MarkTerminated(uint32_t pid, processing::ProcessingContext& context);

		std::unordered_map<uint32_t, CachedProcess> m_cache;

		std::mutex m_mutex;
	};
}