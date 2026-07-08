#pragma once

#include "FlowStatistics.h"
#include "FlowKey.h"
#include <shared/NetworkEvent.h>
#include <timer/TimeTypes.h>

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

	constexpr auto FLOW_IDLE_TIMEOUT = std::chrono::seconds(30);
	
	class Flow
	{
	public:
		explicit Flow(const FlowKey& key, pid_t pid);

		void Update(const NetworkEvent& event);

		const FlowStatistics& GetStatistics() const;

		pid_t GetPID() const;
		bool IsIdle() const;
		Duration GetDuration() const;

		FlowView ToView() const;
	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		TimePoint m_firstSeen;
		TimePoint m_lastSeen;
	};
}