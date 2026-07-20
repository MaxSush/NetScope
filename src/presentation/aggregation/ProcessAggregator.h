#pragma once

#include <vector>
#include <model/ProcessView.h>
#include <model/FlowSnapshot.h>
#include <ProcessManager.h>

namespace netscope
{
	class ProcessAggregator
	{
	public:
		explicit ProcessAggregator(process::ProcessManager& processManager);
		std::vector<flow::ProcessView> Aggregate(const std::vector<flow::FlowSnapshot>& flows) const;

	private:
		process::ProcessManager& m_processManager;
	};
}