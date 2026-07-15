#pragma once

#include <functional>
#include "model/IPAddress.h"
#include "model/Endpoint.h"
#include "model/FlowKey.h"

#include <hash/HashUtils.h>

namespace std
{
	template<>
	struct hash<netscope::flow::IPAddress>
	{
		size_t operator()(const netscope::flow::IPAddress& ip) const
		{
			std::size_t seed = 0;

			netscope::HashCombine(seed, ip.m_family);

			if (ip.IsIPv4())
			{
				netscope::HashCombine(seed, ip.m_address.ipv4);

			}
			else 
			{
				for (uint8_t byte : ip.m_address.ipv6)
				{
					netscope::HashCombine(seed, byte);
				}
			}

			return seed;
		}
	};

	template<>
	struct hash<netscope::flow::Endpoint>
	{
		size_t operator()(const netscope::flow::Endpoint& endpoint) const
		{
			size_t seed = 0;

			netscope::HashCombine(seed, endpoint.address);
			netscope::HashCombine(seed, endpoint.port);

			return seed;
		}
	};

	template<>
	struct hash<netscope::flow::FlowKey>
	{
		size_t operator()(const netscope::flow::FlowKey& key) const
		{
			size_t seed = 0;

			netscope::HashCombine(seed, key.source);
			netscope::HashCombine(seed, key.destination);
			netscope::HashCombine(seed, key.protocol);

			return seed;
		}
	};

}