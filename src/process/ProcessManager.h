#pragma once

#include <model/ProcessInfo.h>
#include <pipeline/IContextProcessor.h>
#include <pipeline/ILifecycleProcessor.h>
#include <tracker/IBpfTracker.h>

#include <config/Config.h>

#include <optional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>

namespace netscope::process
{
	enum class ProcessState
	{
		TERMINATED,
		RUNNING,
		UNKNOWN,
		IGNORED
	};

	struct CachedProcess
	{
		ProcessInfo process;
		ProcessState state;
		std::chrono::steady_clock::time_point lastAccess;

		bool CanEvict(Steady_TimePoint now) const
		{
			return (now - lastAccess) >= Config::Get().processEvictionTimeout;
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

		uint64_t GetBootTimeSeconds() const;

		std::unordered_map<uint32_t, CachedProcess> m_cache;

		std::mutex m_mutex;
	};
}