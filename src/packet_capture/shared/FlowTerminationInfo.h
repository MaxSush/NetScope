#pragma once

#include <optional>
#include "CloseReason.h"

namespace netscope
{
	struct FlowTerminationInfo
	{
		std::optional<CloseReason> closeReason = std::nullopt;
		uint8_t shutdownMask;
		uint32_t kernelError;
	};
}