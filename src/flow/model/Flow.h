#pragma once

#include <model/FlowStatistics.h>
#include <model/FlowKey.h>
#include <model/FlowSnapshot.h>
#include <model/FlowState.h>
#include <event/NetworkEvent.h>

#include <time/TimeUtils.h>

#include <sys/types.h>
#include <optional>


namespace netscope::flow
{
	class Flow
	{
	public:
		explicit Flow(const NetworkEvent& event, const FlowKey& key);

		void Update(const NetworkEvent& event);

		pid_t GetPID() const;
		const FlowKey& GetKey() const;

		bool IsIdle(Steady_TimePoint timeout) const;
		Duration GetIdleTime() const;

		FlowSnapshot GetSnapshot() const;

		inline FlowState GetState() const { return m_snapshot.state; }
		void SetState(FlowState state);

		void SetCloseReason(CloseReason reason);
		void SetTerminationInfo(FlowTerminationInfo info);
		
		inline void SetCloseInfo(FlowTerminationInfo reason) { m_snapshot.terminationInfo = reason; }
		inline std::optional<FlowTerminationInfo> GetCloseReason() const { return m_snapshot.terminationInfo; }

		bool CanEvict(Steady_TimePoint now) const;
		bool HasTcpIdleTimeout(Steady_TimePoint now) const;
		bool HasUdpIdleTimeout(Steady_TimePoint now) const;
	private:
		FlowSnapshot m_snapshot;

		Steady_TimePoint m_lastActivity;
	};
}