#pragma once

#include "FlowStatistics.h"
#include "FlowKey.h"
#include <shared/NetworkEvent.h>
#include <timer/TimeTypes.h>

#include <sys/types.h>

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
		explicit Flow(const FlowKey& key);

		void Update(const NetworkEvent& event);

		const FlowStatistics& GetStatistics() const;

	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		TimePoint m_firstSeen;
		TimePoint m_lastSeen;
	};
}