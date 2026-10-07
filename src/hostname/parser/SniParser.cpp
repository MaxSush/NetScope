#include "SniParser.h"


namespace netscope::hostname
{
	std::optional<std::string> SniParser::Parse(const uint8_t* data, size_t len)
	{
		// Server Name Indication (SNI)
		// very first message a web browser sends to a secure server (a TLS Client Hello) and 
		// extract the Server Name Indication (SNI)
		// The SNI is buried deep inside the "Extensions" layer at the very end.
		
		
		if (len < 44)
		{
			return std::nullopt;
		}

		size_t pos = 0;

		// ── TLS Record Header ──
		// 0x16 = TLS Handshake record
		if (data[pos] != 0x16)
		{
			return std::nullopt;
		}
		pos += 1; // [1-2] TLS version (0x0301=TLS1.0, 0x0303=TLS1.2). Skip.
		pos += 2; // [3-4] Record length. Skip (we trust our buffer length).
		pos += 2;

		// ── Handshake Header ──
		// 0x01 = Client Hello
		if (data[pos] != 0x01)
		{
			return std::nullopt;
		}
		pos += 1; // [6-8] Handshake length. Skip.
		pos += 3; 

		// ── Client Hello Body ──
		// [9-10] TLS version. Skip.
		pos += 2; // [11-42] Random (32 bytes). Skip.
		pos += 32;

		// [43] Session ID length
		if (pos >= len)
		{
			return std::nullopt;
		}
		uint8_t sessionIdLen = data[pos];
		pos += 1 + sessionIdLen;

		// Cipher Suites length (2 bytes)
		if (pos + 2 > len)
		{
			return std::nullopt;
		}
		uint16_t cipherSuitesLen = ReadU16(&data[pos]);
		pos += 2 + cipherSuitesLen;

		// Compression Methods length (1 byte)
		if (pos >= len)
		{
			return std::nullopt;
		}
		uint8_t compMethodsLen = data[pos];
		pos += 1 + compMethodsLen;

		// ── Extensions ──
		if (pos + 2 > len)
		{
			return std::nullopt;
		}
		uint16_t extensionLen = ReadU16(&data[pos]);
		pos += 2;

		if (pos + extensionLen > len)
		{
			extensionLen = static_cast<uint16_t>(len - pos);
		}

		return FindSni(&data[pos], extensionLen);
	}

	uint16_t SniParser::ReadU16(const uint8_t* p)
	{
		return (static_cast<uint16_t>(p[0]) << 8) | p[1];
	}

	std::optional<std::string> SniParser::FindSni(const uint8_t* extData, size_t extLen)
	{
		// Type-Length-Value (TLV). 
		// Every extension starts with 2 bytes for its Type, 2 bytes for its Length, 
		// followed by the actual Value (Data).

		size_t pos = 0;

		while (pos + 4 <= extLen)
		{
			// Reads the Type and Length, advancing the cursor by 4 bytes.
			uint16_t extType = ReadU16(&extData[pos]);
			uint16_t extDataLen = ReadU16(&extData[pos + 2]);
			pos += 4;

			if (pos + extDataLen > extLen)
			{
				break;
			}

			// 0x0000 = Server Name Indication (SNI) extension
			if (extType == 0x0000)
			{
				// SNI extension data:
				// [0-1] SNI list length
				// [2]   Name type (0x00 = hostname)
				// [3-4] Name length
				// [5..] Name value (the hostname!)
				if (extDataLen < 5)
				{
					return std::nullopt;
				}

				// Check if name type is 0x00 (hostname)
				uint8_t nameType = extData[pos + 2];
				if (nameType != 0x00)
				{
					return std::nullopt;
				}

				// Read the name length (2 bytes)
				uint16_t nameLen = ReadU16(&extData[pos + 3]);
				if (pos + 5 + nameLen > extLen)
				{
					return std::nullopt;
				}

				std::string hostname(reinterpret_cast<const char*>(&extData[pos + 5]), nameLen);

				// Validate that the hostname contains only printable ASCII characters
				for (char c : hostname)
				{
					if (c < 0x20 || c > 0x7E)
					{
						return std::nullopt;
					}
				}
				return hostname;
			}
			pos += extDataLen;
		}
		return std::nullopt;
	}
}