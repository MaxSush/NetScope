#pragma once

#include "Direction.h"
#include <model/IPAddress.h>
#include <model/Protocol.h> 

namespace netscope
{
	struct NetworkEvent
	{
		IPAddress sourceAddress;
		uint16_t sourcePort;

		IPAddress destinationAddress;
		uint16_t destinationPort;

		Protocol protocol;

		Direction direction;

		uint32_t bytes;

		pid_t pid{};
		uint32_t tid;
	};
}