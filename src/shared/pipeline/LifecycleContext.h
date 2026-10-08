#pragma once
#include <time/TimeUtils.h>
#include <vector>

namespace netscope::flow
{
	struct FlowResult;
}
namespace netscope::process
{
	struct ProcessResult;
}

namespace netscope::processing
{
	struct LifecycleContext
	{
		explicit LifecycleContext(Steady_TimePoint now) : now(now) {}

		LifecycleContext(const LifecycleContext&) = delete;
		LifecycleContext& operator=(const LifecycleContext&) = delete;

		Steady_TimePoint now;

		std::vector<process::ProcessResult> exitedProcess;

		std::vector<flow::FlowResult> flowChanges;
	};
}