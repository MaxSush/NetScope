#include "IPAddress.h"
#include <arpa/inet.h>

namespace netscope::flow
{
	IPAddress::IPAddress()
		: m_family(AddressFamily::IPv4)
	{
		m_address.ipv4 = 0;
	}

	IPAddress IPAddress::IPv4(uint32_t address)
	{
		IPAddress ip;

		ip.m_address.ipv4 = address;
		ip.m_family = AddressFamily::IPv4;

		return ip;
	}

	IPAddress IPAddress::IPv6(const std::array<uint8_t, 16>& address)
	{
		IPAddress ip;

		ip.m_address.ipv6 = address;
		ip.m_family = AddressFamily::IPv6;

		return ip;
	}

	bool IPAddress::IsIPv4() const
	{
		return m_family == AddressFamily::IPv4;
	}

	bool IPAddress::IsIPv6() const
	{
		return m_family == AddressFamily::IPv6;
	}

	std::string IPAddress::ToString() const
	{
		char buffer[INET6_ADDRSTRLEN];

		if (IsIPv4())
		{
			inet_ntop(AF_INET, &m_address.ipv4, buffer, sizeof(buffer));
		}
		else
		{
			inet_ntop(AF_INET6, m_address.ipv6.data(), buffer, sizeof(buffer));
		}

		return buffer;
	}

	bool IPAddress::operator==(const IPAddress& other) const
	{
		if (m_family != other.m_family)
		{
			return false;
		}
		if (IsIPv4())
		{
			return m_address.ipv4 == other.m_address.ipv4;
		}
		return m_address.ipv6 == other.m_address.ipv6;
	}

	bool IPAddress::operator<(const IPAddress& other) const
	{
		if (m_family != other.m_family)
		{
			return m_family < other.m_family;
		} 
		else 
		{
			if (m_family == AddressFamily::IPv4)
			{
				return m_address.ipv4 < other.m_address.ipv4;

			}
			else 
			{
				return std::lexicographical_compare(
					m_address.ipv6.begin(),
					m_address.ipv6.end(),
					other.m_address.ipv6.begin(),
					other.m_address.ipv6.end()
				);
			}
		}
	}
}