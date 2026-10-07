#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

#include <model/IPAddress.h>

namespace netscope::hostname
{
	struct DnsMapping
	{
		std::string queryDomain;
		std::vector<flow::IPAddress> addresses;
		uint32_t ttl;
	};

	class DnsResponseParser
	{
	public:
		static std::optional<DnsMapping> Parse(const uint8_t* data, size_t len);

	private:
		static std::string ReadName(const uint8_t* packet, size_t packetLen, size_t& offset, int depth = 0);

		static uint16_t ReadU16(const uint8_t* p);
		static uint32_t ReadU32(const uint8_t* p);

	};
}