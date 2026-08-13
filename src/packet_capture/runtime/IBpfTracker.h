#pragma once

#include <cstdint>

namespace netscope
{
	class IBpfTracker
	{
	public:
		virtual ~IBpfTracker() = default;

		virtual void TrackPid(uint32_t pid) const = 0;
		virtual void UntrackPid(uint32_t pid) const = 0;
	};
}