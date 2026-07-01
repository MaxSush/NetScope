
#include "Timer.h"

namespace netscope
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		m_start = std::chrono::steady_clock::now();
	}

	double Timer::Restart()
	{
		double elapsed = ElapsedSeconds();
		Reset();
		return elapsed;
	}

	double Timer::ElapsedSeconds() const
	{
		auto duration = std::chrono::duration<double>(
			std::chrono::steady_clock::now() - m_start
		);
		return duration.count();
	}

	double Timer::ElapsedMilliSeconds() const
	{
		auto duration = std::chrono::duration<double, std::milli>(
			std::chrono::steady_clock::now() - m_start
		);
		return duration.count();
	}
}
