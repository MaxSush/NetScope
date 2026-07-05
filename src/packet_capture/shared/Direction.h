#pragma once

#include <cstdint>

namespace netscope
{
    enum class Direction : uint8_t
    {
        INCOMMING = 0,
        OUTGOING = 1
    };

    inline constexpr const char* ToString(Direction direction)
    {
        switch (direction)
        {
        case Direction::INCOMMING:
            return "INCOMMING";
        case Direction::OUTGOING:
            return "OUTGOING";
        default:
            return "UNKNOWN";
        }
    }
}