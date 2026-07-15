#pragma once
#include <shared/NetworkEvent.h>
#include <ProcessInfo.h>
#include <model/FlowView.h>



namespace netscope::processing
{
	struct FlowUpdatedEvent
	{
		process::ProcessInfo process;
		flow::FlowView flow;
	};
}