#pragma once

#include "Direction.h"
#include <model/IPAddress.h>
#include <model/Protocol.h> 
#include <optional>
#include "NetworkEventType.h"

namespace netscope
{
	struct NetworkEvent
	{
		pid_t pid{};
		pid_t tid{};
		uint64_t bytes;

		NetworkEventType type;
		std::optional<CloseReason> closeReason = std::nullopt;

		flow::Protocol protocol;
		Direction direction;
		flow::AddressFamily family;

		uint16_t sourcePort;
		uint16_t destinationPort;

		flow::IPAddress sourceAddress;
		flow::IPAddress destinationAddress;
	};
}