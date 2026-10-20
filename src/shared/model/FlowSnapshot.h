#pragma once

#include <cstdint>
#include "FlowKey.h"
#include "FlowStatistics.h"
#include "FlowState.h"

#include <event/FlowTerminationInfo.h>
#include <event/NetworkEvent.h>
#include <time/TimeUtils.h>

namespace netscope::flow
{
	struct FlowSnapshot
	{
		FlowSnapshot(const NetworkEvent& event)
		{
			source = Endpoint{ event.sourceAddress, event.sourcePort };
			destination = Endpoint{ event.destinationAddress, event.destinationPort };
			protocol = event.protocol;
			pid = event.pid;
			terminationInfo = event.close;
			ifindex = event.ifindex;
		}

		FlowKey key;

		Endpoint source;
		Endpoint destination;

		Protocol protocol{ Protocol::UNKNOWN };

		pid_t pid;
		FlowStatistics statistics;

		System_TimePoint firstSeen;
		System_TimePoint lastSeen;
		
		Duration IdleTime;

		FlowState state = FlowState::ACTIVE;

		FlowTerminationInfo terminationInfo{};

		uint32_t ifindex = 0;

		inline bool IsIdle(Duration timeout) const
		{
			return IdleTime >= timeout;
		}
	};
}