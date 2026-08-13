#include "Flow.h"

#include <Config.h>

namespace netscope::flow
{
	Flow::Flow(const FlowKey& key, pid_t pid)
		:
		m_key(key),
		m_pid(pid)
	{
		m_lastActivity = Steady_Clock::now();
		m_firstSeen = System_Clock::now();
		m_lastSeen = System_Clock::now();

		m_closeInfo.closeReason = CloseReason::NOT_CLOSED;
	}

	void Flow::Update(const NetworkEvent& event)
	{
		m_lastSeen = System_Clock::now();
		m_lastActivity = Steady_Clock::now();

		if (event.direction == Direction::OUTGOING)
		{
			m_statistics.uploadPackets++;
			m_statistics.uploadBytes += event.bytes;
		} 
		else
		{
			m_statistics.downloadPackets++;
			m_statistics.downloadBytes += event.bytes;
		}
	}

	pid_t Flow::GetPID() const
	{
		return m_pid;
	}

	const FlowKey& Flow::GetKey() const
	{
		return m_key;
	}

	bool Flow::IsIdle(Steady_TimePoint now) const
	{
		return (now - m_lastActivity) >= config::FLOW_IDLE_TIMEOUT;
	}

	Duration Flow::GetIdleTime() const
	{
		return Steady_Clock::now() - m_lastActivity;
	}

	FlowSnapshot Flow::GetSnapshot() const
	{
		FlowSnapshot snapshot;
		snapshot.key = m_key;
		snapshot.pid = m_pid;
		snapshot.statistics = m_statistics;
		snapshot.firstSeen = m_firstSeen;
		snapshot.lastSeen = m_lastSeen;
		snapshot.IdleTime = GetIdleTime();

		snapshot.state = m_state;
		snapshot.closeReason = m_closeInfo.closeReason.value_or(CloseReason::NOT_CLOSED);

		return snapshot;
	}

	void Flow::SetState(FlowState state)
	{
		m_state = state;
		if (state == FlowState::CLOSED)
		{
			m_lastActivity = Steady_Clock::now();
		}
	}

	void Flow::SetCloseReason(CloseReason reason)
	{
		m_closeInfo.closeReason = reason;
	}

	bool Flow::CanEvict(Steady_TimePoint now) const
	{
		return m_state == FlowState::CLOSED && (now - m_lastActivity) >= config::FLOW_EVICTION_TIMEOUT;
	}

	bool Flow::HasTcpIdleTimeout(Steady_TimePoint now) const
	{
		return now - m_lastActivity >= config::FLOW_TCP_IDLE_HARD_TIMEOUT;
	}

}