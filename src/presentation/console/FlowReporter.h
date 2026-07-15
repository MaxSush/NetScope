#pragma once
#include <manager/FlowManager.h>

namespace netscope
{
	class FlowReporter
	{
	public:
		explicit FlowReporter(flow::FlowManager& flowManager);

		void Report();
	private:
		flow::FlowManager& m_flowManager;

	};
}
