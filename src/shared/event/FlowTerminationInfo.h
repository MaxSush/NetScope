#pragma once

#include <optional>
#include "CloseReason.h"

namespace netscope
{
	struct FlowTerminationInfo
	{
		CloseReason closeReason = CloseReason::NOT_CLOSED;
		uint8_t shutdownMask = 0;
		uint32_t kernelError = 0;
	};
}