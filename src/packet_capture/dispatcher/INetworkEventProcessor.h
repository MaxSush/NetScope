#pragma once
#include <shared/NetworkEvent.h>

namespace netscope
{
	class INetworkEventProcessor
	{
	public:
		virtual ~INetworkEventProcessor() = default;
		virtual void Process(const NetworkEvent& event) = 0;
	};

}
