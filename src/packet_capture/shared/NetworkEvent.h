#pragma once

#include "Direction.h"
#include <model/IPAddress.h>
#include <model/Protocol.h> 
#include <optional>
#include "FlowTerminationInfo.h"

namespace netscope
{
	enum class NetworkEventType : uint8_t
	{
		PACKET,
		CONNECTION_CLOSED,
		PROCESS_EXITED
	};

	struct NetworkEvent
	{
		pid_t pid{};
		pid_t tid{};
		uint64_t bytes;

		NetworkEventType type;
		FlowTerminationInfo close;

		flow::Protocol protocol;
		Direction direction;
		flow::AddressFamily family;

		uint16_t sourcePort;
		uint16_t destinationPort;

		flow::IPAddress sourceAddress;
		flow::IPAddress destinationAddress;
	};
}