#pragma once
#include <model/FlowSnapshot.h>
#include <ProcessInfo.h>

namespace netscope::processing
{
	struct FlowCreatedEvent
	{
		flow::FlowSnapshot flow;
		process::ProcessInfo process;
	};
}