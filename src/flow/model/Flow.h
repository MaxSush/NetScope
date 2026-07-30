#pragma once

#include "FlowStatistics.h"
#include "FlowKey.h"
#include <shared/NetworkEvent.h>
#include <time/TimeUtils.h>

#include <sys/types.h>
#include "FlowSnapshot.h"
#include "FlowState.h"
#include <optional>


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
		bool IsIdle(Steady_TimePoint timeout) const;
		Duration GetIdleTime() const;
		Duration GetFlowLifetime() const;

		FlowSnapshot GetSnapshot() const;

		inline FlowState GetState() const { return m_state; }
		inline void SetState(FlowState state) { m_state = state; }
		void SetCloseReason(CloseReason reason) { m_closeReason = reason; }
		std::optional<CloseReason> GetCloseReason() const { return m_closeReason; }

		bool CanArchive(Steady_TimePoint now) const;
	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		Steady_TimePoint m_lastActivity;

		System_TimePoint m_firstSeen;
		System_TimePoint m_lastSeen;

		FlowState m_state = FlowState::ACTIVE;
		std::optional<CloseReason> m_closeReason;
	};
}