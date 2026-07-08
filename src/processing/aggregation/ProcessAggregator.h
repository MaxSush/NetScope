#pragma once

#include <vector>
#include <model/ProcessView.h>
#include <model/FlowView.h>
#include <ProcessCache.h>

namespace netscope
{
	class ProcessAggregator
	{
	public:
		explicit ProcessAggregator(ProcessCache& cache);
		std::vector<ProcessView> Aggregate(const std::vector<FlowView>& flows) const;

	private:
		ProcessCache& m_processCache;
	};
}