#include "DnsResponseParser.h"
#include <algorithm>
#include <cstring>

namespace netscope::hostname
{
	std::optional<DnsMapping> DnsResponseParser::Parse(const uint8_t* data, size_t len)
	{
		// DNS packet structure :
		//	Header (Always 12 bytes)
		//	Questions(What was asked ? )
		//	Answers(The IP addresses)

		// Check if the packet is at least 12 bytes long (the size of the DNS header)
		if (len < 12)
		{
			return std::nullopt;
		}

		// Read the flags from the DNS header (bytes 2 and 3) 
		// We check highest bit which is Response and lowest 4 bit are Response code
		// If the highest bit is not set, it means it's not a response packet
		// If the lowest 4 bits are not zero, it means there was an error in the response
		uint16_t flags = ReadU16(&data[2]);
		if (!(flags & 0x8000))
		{
			return std::nullopt;
		}
		if ((flags & 0x000F) != 0)
		{
			return std::nullopt;
		}

		// Read the question count and answer count from the DNS header (bytes 4->7)
		uint16_t qdCount = ReadU16(&data[4]);
		uint16_t anCount = ReadU16(&data[6]);

		if (qdCount == 0 || anCount == 0)
		{
			return std::nullopt;
		}

		// Start reading the question section, which starts immediately after the 12-byte header
		size_t offset = 12;

		// Read the domain name from the question section
		std::string queryDomain = ReadName(data, len, offset);
		if (queryDomain.empty())
		{
			return std::nullopt;
		}

		// it repeats the question after, Skip the QTYPE and QCLASS fields (4 bytes) in the question section
		offset += 4;

		// Skip any additional questions
		// TODO: Ignore Afterwards
		for (uint16_t i = 1; i < qdCount; ++i)
		{
			ReadName(data, len, offset);
			offset += 4;
			if (offset > len)
			{
				return std::nullopt;
			}
		}

		DnsMapping mapping;
		mapping.queryDomain = queryDomain;
		mapping.ttl = UINT32_MAX;

		// Read the answer section, which contains the resource records (RRs)
		for (uint16_t i = 0; i < anCount; ++i)
		{
			if (offset >= len)
			{
				break;
			}

			// Skip the domain name from the answer section
			ReadName(data, len, offset);

			if (offset + 10 > len)
			{
				break;
			}

			uint16_t rrType = ReadU16(&data[offset]);			// Type of the resource record (e.g., A, AAAA, CNAME)
			//uint16_t rrClass = ReadU16(&data[offset + 2]);	// Skip
			uint32_t ttl = ReadU32(&data[offset + 4]);			// Time to cache for the resource record in system
			uint16_t rdLen = ReadU16(&data[offset + 8]);		// Length of the resource data (e.g., 4 bytes for IPv4, 16 bytes for IPv6)
			offset += 10;

			if (offset + rdLen > len)
			{
				break;
			}

			mapping.ttl = std::min(mapping.ttl, ttl);
			
			// Type 1 is an "A Record" (IPv4), an IPv4 address. Copies those 4 bytes.
			if (rrType == 1 && rdLen == 4)
			{
				uint32_t ipv4;
				std::memcpy(&ipv4, &data[offset], 4);
				mapping.addresses.push_back(flow::IPAddress::IPv4(ipv4));
			}
			// Type 28 is an "AAAA Record" (IPv6), an IPv6 address. Copies those 16 bytes.
			else if (rrType == 28 && rdLen == 16)
			{
				std::array<uint8_t, 16> ipv6;
				std::memcpy(ipv6.data(), &data[offset], 16);
				mapping.addresses.push_back(flow::IPAddress::IPv6(ipv6));
			}

			// Move the offset forward by the length of the resource data to continue parsing the next record
			offset += rdLen;
		}

		if (mapping.addresses.empty())
		{
			return std::nullopt;
		}
		if (mapping.ttl == UINT32_MAX)
		{
			mapping.ttl = 300;
		}

		return mapping;
	}

	std::string DnsResponseParser::ReadName(const uint8_t* packet, size_t packetLen, size_t& offset, int depth)
	{
		// parse domain name e.g "[6] g o o g l e [3] c o m [0]"
		
		// Handle potential infinite loops due to malformed packets
		if (depth > 10) 
		{
			return "";
		}

		std::string name;
		while (offset < packetLen)
		{
			// Read the length of the next label
			uint8_t labelLen = packet[offset];
			if (labelLen == 0)
			{
				offset += 1; // Skip the null terminator
				break;
			}

			// Compression pointer : 2-byte "pointer" pointing to another location in the packet
			// Check for pointer (compression pointer) in the label : top 2 bits are 11
			if ((labelLen & 0xC0) == 0xC0)
			{
				if (offset + 1 >= packetLen)
				{
					return "";
				}

				// strips off the 11 bits using & 0x3F, shifts it, and combines it with the next byte 
				// to calculate the exact location
				size_t ptrOffset = ((labelLen & 0x3F) << 8) | packet[offset + 1];
				offset += 2;

				// Recursively read the name from the pointer location
				std::string rest = ReadName(packet, packetLen, ptrOffset, depth + 1);

				if (!name.empty())
				{
					name += ".";
				}
				name += rest;
				return name;
			}

			offset += 1;
			if (offset + labelLen > packetLen)
			{
				return "";
			}

			// Append the label to the name, adding a dot if necessary
			if (!name.empty())
			{
				name += ".";
			}
			name.append(reinterpret_cast<const char*>(packet + offset), labelLen);
			offset += labelLen;
		}
		return name;
	}

	uint16_t DnsResponseParser::ReadU16(const uint8_t* p)
	{
		// Combine two bytes into a 16-bit unsigned integer in big-endian order
		return static_cast<uint16_t>((p[0] << 8) | p[1]);
	}

	uint32_t DnsResponseParser::ReadU32(const uint8_t* p)
	{
		// Combine four bytes into a 32-bit unsigned integer in big-endian order
		return (static_cast<uint32_t>(p[0]) << 24) |
			(static_cast<uint32_t>(p[1]) << 16) |
			(static_cast<uint32_t>(p[2]) << 8) |
			(static_cast<uint32_t>(p[3]));
	}
}