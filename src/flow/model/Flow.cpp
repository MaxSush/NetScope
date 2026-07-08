#include "Flow.h"
#include "Flow.h"
#include "Flow.h"
#include "Flow.h"

namespace netscope
{
	Flow::Flow(const FlowKey& key, pid_t pid)
		:
		m_key(key),
		m_pid(pid)
	{
		m_firstSeen = Clock::now();
		m_lastSeen = Clock::now();
	}

	void Flow::Update(const NetworkEvent& event)
	{
		m_lastSeen = Clock::now();

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
		return Clock::now() - m_lastSeen;
	}

	Duration Flow::GetFlowLifetime() const
	{
		 return Clock::now() - m_lastSeen;
	}

	FlowView Flow::ToView() const
	{
		FlowView view;
		view.key = m_key;
		view.pid = m_pid;
		view.statistics = m_statistics;
		view.firstSeen = m_firstSeen;
		view.lastSeen = m_lastSeen;

		return view;
	}

	const FlowStatistics& Flow::GetStatistics() const
	{
		return m_statistics;
	}
}