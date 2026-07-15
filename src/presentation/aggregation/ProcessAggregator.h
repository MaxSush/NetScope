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
		explicit ProcessAggregator(process::ProcessCache& cache);
		std::vector<flow::ProcessView> Aggregate(const std::vector<flow::FlowView>& flows) const;

	private:
		process::ProcessCache& m_processCache;
	};
}