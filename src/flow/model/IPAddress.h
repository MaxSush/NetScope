#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace netscope::flow
{
	enum class AddressFamily : uint8_t
	{
		IPv4,
		IPv6
	};

	class IPAddress
	{
		friend struct std::hash<IPAddress>;

	public:
		IPAddress();

		static IPAddress IPv4(uint32_t address);
		static IPAddress IPv6(const std::array<uint8_t, 16>& address);

		bool IsIPv4() const;
		bool IsIPv6() const;

		std::string ToString() const;

		bool operator==(const IPAddress& other) const;
		bool operator<(const IPAddress& other) const;

	private:

		union AddressStorage
		{
			uint32_t ipv4;
			std::array<uint8_t, 16> ipv6;

			AddressStorage()
				: ipv4(0)
			{

			}
		};

		AddressStorage m_address;
		AddressFamily m_family;
	};
}

namespace std
{
	template<>
	struct hash<netscope::flow::IPAddress>;
}