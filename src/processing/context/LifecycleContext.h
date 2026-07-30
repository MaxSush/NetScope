#pragma once
#include <time/TimeUtils.h>
#include <vector>

namespace netscope::flow
{
	enum class FlowChange;
	class FlowResult;
}

namespace netscope::processing
{
	struct LifecycleContext
	{
		explicit LifecycleContext(Steady_TimePoint now) : now(now) {}

		LifecycleContext(const LifecycleContext&) = delete;
		LifecycleContext& operator=(const LifecycleContext&) = delete;

		Steady_TimePoint now;

		std::vector<flow::FlowResult> flowChanges;
	};
}