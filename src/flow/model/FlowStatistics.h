#pragma once

#include <cstdint>

namespace netscope
{
	struct FlowStatistics
	{
		uint64_t uploadBytes{};
		uint64_t downloadBytes{};
		
		uint64_t uploadPackets{};
		uint64_t downloadPackets{};
	};
}