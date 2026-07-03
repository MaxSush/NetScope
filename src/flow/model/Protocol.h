#pragma once

#include <cstdint>

namespace netscope
{
	enum class Protocol : uint8_t
	{
		UNKNOWN = 0,
		
		ICMP = 1,
		TCP = 6,
		UDP = 17,
		ICMPv6 = 58
	};

	constexpr const char* ToString(Protocol protocol)
	{
		switch (protocol)
		{
		case Protocol::ICMP:
			return "ICMP";
		case Protocol::TCP:
			return "TCP";
		case Protocol::UDP:
			return "UDP";
		case Protocol::ICMPv6:
			return "ICPMv6";
		default:
			return "UNKNOWN";
		}
	}
}