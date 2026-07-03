#include "NetworkAggregator.h"
#include <iostream>

namespace netscope
{
	NetworkAggregator::NetworkAggregator(EventQueue<NetworkEvent>& queue)
		:
		m_queue(queue)
	{

	}

	void NetworkAggregator::Run()
	{
		while (m_running)
		{
			try 
			{
				NetworkEvent event = m_queue.Pop();
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
			} 
			catch (...)
			{
				break;
			}
		}
	}

	void NetworkAggregator::Stop()
	{
		m_running = false;
	}

	std::unordered_map<uint64_t, NetworkStatistics> NetworkAggregator::GetSnapshot()
	{
		std::lock_guard lock(m_mutex);

		return m_statistics;
	}

}