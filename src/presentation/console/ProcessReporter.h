#pragma once
#include <aggregation/ProcessAggregator.h>
#include <manager/FlowManager.h>

namespace netscope
{
	class ProcessReporter
	{
	public:
		explicit ProcessReporter(ProcessAggregator& processAggregator, flow::FlowManager& flowManager);

		void Report();
	private:
		ProcessAggregator& m_processAggregator;
		flow::FlowManager& m_flowManager;
	};
}