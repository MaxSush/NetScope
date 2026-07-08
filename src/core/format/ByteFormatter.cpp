#include "ByteFormatter.h"
#include <array>
#include <fmt/format.h>

namespace netscope
{
	std::string ByteFormatter::Format(std::uint64_t bytes, size_t precision)
	{
		constexpr std::array<const char*, 5> units =
		{
			"B",
			"KB",
			"MB",
			"GB",
			"TB"
		};

		double value = static_cast<double>(bytes);
		size_t unit = 0;

		while (value >= 1024.0 && unit < units.size() - 1)
		{
			value /= 1024.0;
			++unit;
		}
		if (unit == 0)
		{
			return fmt::format("{} {}", bytes, units[unit]);
		}
		return fmt::format("{:.{}f} {}", value, precision, units[unit]);
	}
}
