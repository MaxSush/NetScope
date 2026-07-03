#pragma once

#include "statistics/NetworkAggregator.h"

#include <ProcessCache.h>

namespace netscope
{
	class StatisticsReporter
	{
	public:
		StatisticsReporter(NetworkAggregator& aggregator, ProcessCache& m_processCache);

		void Report();
	private:
		NetworkAggregator& m_aggregator;
		ProcessCache& m_processCache;
	};
}