#pragma once

#include <cstdint>

namespace netscope
{
	struct NetworkStatistics
	{
		uint64_t uploaded = 0;
		uint64_t downloaded = 0;
	};
}