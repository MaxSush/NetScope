#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace netscope::hostname
{
	class SniParser
	{
	public:
		static std::optional<std::string> Parse(const uint8_t* data, size_t len);

	private:
		static uint16_t ReadU16(const uint8_t* p);

		static std::optional<std::string> FindSni(const uint8_t* extData, size_t extLen);

	};
}