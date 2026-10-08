#include "FlowKey.h"

namespace netscope::flow
{
	FlowKey FlowKey::Create(Endpoint source, Endpoint destination, Protocol protocol)
	{
		if (destination < source)
		{
			std::swap(source, destination);
		}
		return { source, destination, protocol };
	}

	bool FlowKey::operator==(const FlowKey& other) const
	{
		return
			source == other.source &&
			destination == other.destination &&
			protocol == other.protocol;
	}
}