#pragma once

#include "ProcessInfo.h"
#include <pipeline/IContextProcessor.h>
#include <lifecycle/ILifecycleProcessor.h>

#include <optional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>


namespace netscope::process
{
	struct CachedProcess
	{
		ProcessInfo process;
		std::chrono::steady_clock::time_point lastAccess;
	};

	class ProcessManager :	public processing::IContextProcessor,
							public processing::ILifecycleProcessor
	{
	public:
		ProcessManager() = default;

		void Process(processing::ProcessingContext& context) override;

		// Inherited via ILifecycleProcessor
		void Process(processing::LifecycleContext& context) override;
		void CleanUpExpired(Steady_TimePoint now) override;

		std::vector<ProcessInfo> GetProcesses() const;
		std::optional<ProcessResult> Resolve(uint32_t pid);


		void CleanUp(std::chrono::seconds maxAge);


	private:
		bool IsPidDirectory(const std::string& name) const;
		std::optional<ProcessInfo> ReadProcess(const uint32_t pid) const;


		std::unordered_map<uint32_t, CachedProcess> m_cache;

		std::mutex m_mutex;

	};
}