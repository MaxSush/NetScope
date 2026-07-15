#pragma once

#include <chrono>
#include "ProcessInfo.h"

namespace netscope::process
{
	struct CachedProcess
	{
		ProcessInfo process;
		std::chrono::steady_clock::time_point lastAccess;
	};
}