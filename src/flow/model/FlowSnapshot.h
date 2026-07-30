#pragma once

#include <cstdint>
#include "FlowKey.h"
#include "FlowStatistics.h"
#include "model/FlowState.h"
#include <time/TimeUtils.h>

namespace netscope::flow
{
	struct FlowSnapshot
	{
		FlowKey key;
		pid_t pid;
		FlowStatistics statistics;

		System_TimePoint firstSeen;
		System_TimePoint lastSeen;
		
		Duration IdleTime;

		FlowState state;

		CloseReason closeReason;

		inline bool IsIdle(Duration timeout) const
		{
			return IdleTime >= timeout;
		}
	};
}