#include "Flow.h"

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

	bool Flow::IsIdle(Duration timeout) const
	{
		return (GetIdleTime() >= timeout);
	}

	Duration Flow::GetIdleTime() const
	{
		return Steady_Clock::now() - m_lastActivity;
	}

	Duration Flow::GetFlowLifetime() const
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

		return snapshot;
	}

}