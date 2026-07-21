#pragma once

#include <model/FlowSnapshot.h>

namespace netscope::processing
{
	struct FlowUpdatedEvent
	{
		flow::FlowSnapshot flowSnapshot;
	};
}