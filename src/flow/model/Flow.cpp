#include "Flow.h"

#include <config/Config.h>

namespace netscope::flow
{
	Flow::Flow(const NetworkEvent& event, const FlowKey& key)
		: m_snapshot(event)
	{
		m_lastActivity = Steady_Clock::now();
		m_snapshot.firstSeen = System_Clock::now();
		m_snapshot.lastSeen = System_Clock::now();
		m_snapshot.key = key;
	}

	void Flow::Update(const NetworkEvent& event)
	{
		m_snapshot.lastSeen = System_Clock::now();
		m_lastActivity = Steady_Clock::now();

		if (event.direction == Direction::OUTGOING)
		{
			m_snapshot.statistics.uploadPackets++;
			m_snapshot.statistics.uploadBytes += event.bytes;
		} 
		else
		{
			m_snapshot.statistics.downloadPackets++;
			m_snapshot.statistics.downloadBytes += event.bytes;
		}
	}

	pid_t Flow::GetPID() const
	{
		return m_snapshot.pid;
	}

	const FlowKey& Flow::GetKey() const
	{
		return m_snapshot.key;
	}

	bool Flow::IsIdle(Steady_TimePoint now) const
	{
		return (now - m_lastActivity) >= Config::Get().flowIdleTimeout;
	}

	Duration Flow::GetIdleTime() const
	{
		return Steady_Clock::now() - m_lastActivity;
	}

	FlowSnapshot Flow::GetSnapshot() const
	{
		auto snapshot = m_snapshot;
		snapshot.IdleTime = GetIdleTime();

		return snapshot;
	}

	void Flow::SetState(FlowState state)
	{
		m_snapshot.state = state;
		if (state == FlowState::CLOSED)
		{
			m_lastActivity = Steady_Clock::now();
		}
	}

	void Flow::SetCloseReason(CloseReason reason)
	{
		m_snapshot.terminationInfo.closeReason = reason;
	}

	void Flow::SetTerminationInfo(FlowTerminationInfo info)
	{
		m_snapshot.terminationInfo = info;
	}

	bool Flow::CanEvict(Steady_TimePoint now) const
	{
		return m_snapshot.state == FlowState::CLOSED && (now - m_lastActivity) >= Config::Get().flowEvictionTimeout;
	}

	bool Flow::HasTcpIdleTimeout(Steady_TimePoint now) const
	{
		return now - m_lastActivity >= Config::Get().flowTcpIdleHardTimeout;
	}

	bool Flow::HasUdpIdleTimeout(Steady_TimePoint now) const
	{
		return (now - m_lastActivity) >= Config::Get().flowUdpIdleCloseTimeout;
	}

}