#pragma once

#include <chrono>

namespace netscope
{
	class Timer
	{
	public:
		Timer();
		void Reset();
		double Restart();
		double ElapsedSeconds() const;
		double ElapsedMilliSeconds() const;

	private:
		std::chrono::steady_clock::time_point m_start;
	};
}