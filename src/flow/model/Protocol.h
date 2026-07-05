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

	inline Protocol ToProtocol(uint8_t val)
	{
		switch (val)
		{
		case 1:
			return Protocol::ICMP;
		case 6:
			return Protocol::TCP;
		case 17:
			return Protocol::UDP;
		case 58:
			return Protocol::ICMPv6;
		default:
			return Protocol::UNKNOWN;
		}
	}

	inline constexpr const char* ToProtocolString(Protocol protocol)
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