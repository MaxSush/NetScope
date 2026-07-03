#include "Flow.h"

namespace netscope
{
	Flow::Flow(const FlowKey& key)
		:
		m_key(key)
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

	const FlowStatistics& Flow::GetStatistics() const
	{
		return m_statistics;
	}
}