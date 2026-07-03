#pragma once

#include "statistics/StatisticsAggregator.h"

#include <ProcessCache.h>

namespace netscope
{
	class StatisticsReporter
	{
	public:
		StatisticsReporter(StatisticsAggregator& aggregator, ProcessCache& m_processCache);

		void Report();
	private:
		StatisticsAggregator& m_aggregator;
		ProcessCache& m_processCache;
	};
}