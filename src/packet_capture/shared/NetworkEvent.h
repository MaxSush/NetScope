#pragma once

#include "Direction.h"
#include <model/IPAddress.h>
#include <model/Protocol.h> 

namespace netscope
{
	struct NetworkEvent
	{
		pid_t pid{};
		pid_t tid{};
		uint64_t bytes;

		flow::Protocol protocol;
		Direction direction;
		flow::AddressFamily family;

		uint16_t sourcePort;
		uint16_t destinationPort;

		flow::IPAddress sourceAddress;
		flow::IPAddress destinationAddress;
	};
}