#pragma once

#include "core/event/EventQueue.h"
#include "shared/NetworkEvent.h"
#include "NetworkStatistics.h"
#include <unordered_map>

namespace netscope
{
	class NetworkAggregator
	{
	public:
		explicit NetworkAggregator(EventQueue<NetworkEvent>& queue);

		void Run();
		void Stop();

		std::unordered_map<uint64_t, NetworkStatistics> GetSnapshot();

	private:
		EventQueue<NetworkEvent>& m_queue;
		bool m_running = true;

		std::mutex m_mutex;

		std::unordered_map<uint64_t, NetworkStatistics> m_statistics;

	};
}