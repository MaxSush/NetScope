#pragma once

#include <cstdint>

namespace netscope::process
{
	struct SocketInfo
	{
		uint32_t pid;
		uint64_t inode;
	};
}