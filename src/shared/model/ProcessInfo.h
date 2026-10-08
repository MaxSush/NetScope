#pragma once

#include <cstdint>
#include <string>
#include <optional>

namespace netscope::process
{
	struct ProcessInfo
	{
		uint32_t pid;
		uint64_t start_time;
		std::optional<int64_t> end_time;
		std::string name;
		std::string exe_path;
	};
}