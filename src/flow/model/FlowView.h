#pragma once

#include <cstdint>
#include "FlowKey.h"
#include "FlowStatistics.h"
#include <time/TimeUtils.h>

namespace netscope::flow
{
	struct FlowView
	{
		FlowKey key;
		pid_t pid;
		FlowStatistics statistics;

		System_TimePoint firstSeen;
		System_TimePoint lastSeen;
		
		Duration IdleTime;

		inline bool IsIdle(Duration timeout) const
		{
			return IdleTime >= timeout;
		}
	};
}