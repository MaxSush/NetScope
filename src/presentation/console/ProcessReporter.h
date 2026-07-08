#pragma once
#include <aggregation/ProcessAggregator.h>
#include <manager/FlowManager.h>

namespace netscope
{
	class ProcessReporter
	{
	public:
		explicit ProcessReporter(ProcessAggregator& processAggregator, FlowManager& flowManager);

		void Report();
	private:
		ProcessAggregator& m_processAggregator;
		FlowManager& m_flowManager;
	};
}