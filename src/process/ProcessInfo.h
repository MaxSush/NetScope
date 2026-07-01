#pragma once

#include <cstdint>
#include <string>

namespace netscope
{
	struct ProcessInfo
	{
		uint32_t pid;
		uint64_t start_time;
		std::string name;
		std::string exe_path;
	};
}