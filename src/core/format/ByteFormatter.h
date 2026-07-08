#pragma once
#include <cstdint>
#include <string>

namespace netscope
{
	class ByteFormatter
	{
	public:
		static std::string Format(std::uint64_t bytes, size_t precision = 2);
	};
}