#pragma once
#include <model/FlowSnapshot.h>
#include <model/ProcessInfo.h>

namespace netscope::processing
{
	struct FlowCreatedEvent
	{
		flow::FlowSnapshot flowSnapshot;
		process::ProcessInfo process;
	};
}