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

		inline FlowStatistics& operator+=(const FlowStatistics& other)
		{
			this->uploadBytes += other.uploadBytes;
			this->downloadBytes += other.downloadBytes;

			this->uploadPackets += other.uploadPackets;
			this->downloadPackets += other.downloadPackets;

			return *this;
		}
	};
}