#pragma once

#include <cstdint>

namespace netscope
{
    enum class Direction : uint8_t
    {
        INCOMMING = 0,
        OUTGOING = 1
    };

    struct PacketEvent
    {
        uint32_t pid;
        uint32_t tid;

        uint64_t bytes;

        Direction direction;

        uint8_t padding[7];
    };

    static_assert(sizeof(PacketEvent) == 24);
}