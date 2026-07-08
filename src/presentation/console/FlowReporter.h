#pragma once
#include <manager/FlowManager.h>

namespace netscope
{
	class FlowReporter
	{
	public:
		explicit FlowReporter(FlowManager& flowManager);

		void Report();
	private:
		FlowManager& m_flowManager;

	};
}
