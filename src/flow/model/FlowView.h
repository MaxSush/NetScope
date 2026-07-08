#pragma once

#include <cstdint>
#include "FlowKey.h"
#include "FlowStatistics.h"
#include <timer/TimeTypes.h>

namespace netscope
{
	struct FlowView
	{
		FlowKey key;
		pid_t pid;
		FlowStatistics statistics;

		TimePoint firstSeen;
		TimePoint lastSeen;
	};
}