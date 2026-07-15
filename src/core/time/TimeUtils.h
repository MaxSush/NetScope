#pragma once

#include <chrono>
#include <cstdint>

namespace netscope
{
	using Steady_Clock = std::chrono::steady_clock;
	using System_Clock = std::chrono::system_clock;
	using Steady_TimePoint = Steady_Clock::time_point;
	using System_TimePoint = System_Clock::time_point;

	using Duration = Steady_Clock::duration;

	using Milliseconds = std::chrono::milliseconds;

	inline int64_t ToUnixMilliseconds(System_TimePoint time)
	{
		return std::chrono::duration_cast<Milliseconds>(time.time_since_epoch()).count();
	}

	inline System_TimePoint FromUnixMilliseconds(int64_t value)
	{
		return System_TimePoint(Milliseconds(value));
	}

	//inline std::string Format(System_TimePoint time)
	//{

	//}
}




