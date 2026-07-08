#pragma once
#include <sys/types.h>
#include <string>
#include <cstdint>
#include <model/FlowStatistics.h>

namespace netscope
{
	struct ProcessView
	{
		pid_t pid;
		std::string processName;

		FlowStatistics statistics;

		size_t activeConnections = 0;
		size_t idleConnections = 0;
	};
}