#pragma once
#include <time/TimeUtils.h>
#include <vector>

namespace netscope::flow
{
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

		std::vector<uint32_t> exitedPids;

		std::vector<flow::FlowResult> flowChanges;
	};
}