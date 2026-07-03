#pragma once

#include <cstddef>
#include <functional>

namespace netscope
{
	namespace hash
	{
		constexpr std::size_t Magic = 0x9e3779b9;
	}

	template<typename T>
	inline void HashCombine(std::size_t& seed, const T& value)
	{
		seed ^= std::hash<T>{}(value)+hash::Magic + (seed << 6) + (seed >> 2);
	}
}
