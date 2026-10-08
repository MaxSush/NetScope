#pragma once

#include <cstdint>
#include "Endpoint.h"
#include "Protocol.h"

namespace netscope::flow
{
	struct FlowKey
	{
		friend struct std::hash<FlowKey>;

		Endpoint source;
		Endpoint destination;

		Protocol protocol{ Protocol::UNKNOWN };

		static FlowKey Create(Endpoint source, Endpoint destination, Protocol protocol);

		bool operator==(const FlowKey& other) const;
	};
}

namespace std
{
	template<>
	struct hash<netscope::flow::FlowKey>;
}