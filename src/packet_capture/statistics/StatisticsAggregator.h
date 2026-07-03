#pragma once

#include <dispatcher/INetworkEventProcessor.h>
#include "NetworkStatistics.h"


#include <unordered_map>
#include <mutex>

namespace netscope
{
	class StatisticsAggregator : public INetworkEventProcessor
	{
	public:
		StatisticsAggregator() = default;

		void Process(const NetworkEvent& event) override;

		std::unordered_map<uint64_t, NetworkStatistics> GetSnapshot();

	private:
		std::mutex m_mutex;

		std::unordered_map<uint64_t, NetworkStatistics> m_statistics;

	};
}