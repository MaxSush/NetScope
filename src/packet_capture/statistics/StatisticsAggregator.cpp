#include "StatisticsAggregator.h"

namespace netscope
{
	void StatisticsAggregator::Process(const NetworkEvent& event)
	{
		std::lock_guard lock(m_mutex);
		auto& stats = m_statistics[event.pid];
		if (event.direction == Direction::INCOMMING)
		{
			stats.downloaded += event.bytes;
		}
		else
		{
			stats.uploaded += event.bytes;
		}
	}

	
	std::unordered_map<uint64_t, NetworkStatistics> StatisticsAggregator::GetSnapshot()
	{
		std::lock_guard lock(m_mutex);

		return m_statistics;
	}

}