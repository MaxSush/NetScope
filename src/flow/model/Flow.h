#pragma once

#include "FlowStatistics.h"
#include "FlowKey.h"
#include <shared/NetworkEvent.h>
#include <time/TimeUtils.h>

#include <sys/types.h>
#include "FlowView.h"

namespace netscope
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

		const FlowStatistics& GetStatistics() const;

		const FlowKey& GetFlowKey() const;

		pid_t GetPID() const;
		bool IsIdle(Duration timeout) const;
		Duration GetIdleTime() const;
		Duration GetFlowLifetime() const;

		FlowView ToView() const;
	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		Steady_TimePoint m_lastActivity;

	public:
		System_TimePoint m_firstSeen;
		System_TimePoint m_lastSeen;
	};
}