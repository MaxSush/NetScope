#pragma once

#include "Direction.h"

namespace netscope
{
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