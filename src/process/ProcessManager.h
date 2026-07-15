#pragma once

#include "ProcessInfo.h"

#include <optional>
#include <vector>


namespace netscope::process
{
	class ProcessManager
	{
	public:
		ProcessManager() = default;
		std::vector<ProcessInfo> GetProcesses() const;
		std::optional<ProcessInfo> FindProcess(uint32_t pid) const;

	private:
		bool IsPidDirectory(const std::string& name) const;
		ProcessInfo ReadProcess(const uint32_t pid) const;
	};
}