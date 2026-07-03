#pragma once

#include "Direction.h"

namespace netscope
{
	struct NetworkEvent
	{
		uint32_t pid;
		uint32_t tid;
		uint64_t bytes;
		Direction direction;
	};
}