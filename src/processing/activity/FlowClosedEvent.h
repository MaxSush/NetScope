#pragma once

#include <model/FlowSnapshot.h>

namespace netscope::processing
{
	struct FlowClosedEvent
	{
		flow::FlowSnapshot flowSnapshot;
	};
}