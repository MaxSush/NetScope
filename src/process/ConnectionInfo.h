#pragma once

#include <cstdint>
#include "ProcessInfo.h"

namespace netscope
{
	struct ConnectionInfo
	{
		uint64_t inode;
		ProcessInfo process;
	};
}