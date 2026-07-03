#pragma once

#include <cstdint>

namespace netscope
{
    enum class Direction : uint8_t
    {
        INCOMMING = 0,
        OUTGOING = 1
    };
}