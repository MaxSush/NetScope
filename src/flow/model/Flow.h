#pragma once

#include "FlowStatistics.h"
#include "FlowKey.h"
#include <shared/NetworkEvent.h>
#include <time/TimeUtils.h>

#include <sys/types.h>
#include "FlowSnapshot.h"

namespace netscope::flow
{
	/*
		Flow
		│
		├── Identity
		├── Statistics
		├── Ownership
		└── Lifetime
	*/
	
	class Flow
	{
	public:
		explicit Flow(const FlowKey& key, pid_t pid);

		void Update(const NetworkEvent& event);

		pid_t GetPID() const;
		bool IsIdle(Duration timeout) const;
		Duration GetIdleTime() const;
		Duration GetFlowLifetime() const;

		FlowSnapshot GetSnapshot() const;
	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		Steady_TimePoint m_lastActivity;

		System_TimePoint m_firstSeen;
		System_TimePoint m_lastSeen;
	};
}