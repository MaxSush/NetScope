#pragma once

#include <cstdint>
#include "ProcessInfo.h"

namespace netscope::process
{
	struct ConnectionInfo
	{
		uint64_t inode;
		ProcessInfo process;
	};
}