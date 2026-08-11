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
	class Flow
	{
	public:
		explicit Flow(const FlowKey& key, pid_t pid);

		void Update(const NetworkEvent& event);

		pid_t GetPID() const;
		const FlowKey& GetKey() const;

		bool IsIdle(Steady_TimePoint timeout) const;
		Duration GetIdleTime() const;
		Duration GetFlowLifetime() const;

		FlowSnapshot GetSnapshot() const;

		inline FlowState GetState() const { return m_state; }
		void SetState(FlowState state);

		void SetCloseReason(CloseReason reason);
		
		inline void SetCloseInfo(FlowTerminationInfo reason) { m_closeInfo = reason; }
		inline std::optional<FlowTerminationInfo> GetCloseReason() const { return m_closeInfo; }

		bool CanEvict(Steady_TimePoint now) const;
		bool HasTcpIdleTimeout(Steady_TimePoint now) const;
	private:
		FlowKey m_key;

		pid_t m_pid;

		FlowStatistics m_statistics;

		Steady_TimePoint m_lastActivity;

		System_TimePoint m_firstSeen;
		System_TimePoint m_lastSeen;

		FlowState m_state = FlowState::ACTIVE;
		FlowTerminationInfo m_closeInfo;
	};
}