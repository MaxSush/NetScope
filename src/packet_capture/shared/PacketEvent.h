#pragma once

#include "Direction.h"

#include <model/IPAddress.h>
#include <model/Protocol.h>

namespace netscope
{
    //struct PacketEvent
    //{
    //    uint32_t pid;
    //    uint32_t tid;

    //    uint64_t bytes;

    //    Direction direction;

    //    uint8_t padding[7];
    //};

    //static_assert(sizeof(PacketEvent) == 24);

    struct PacketEvent
    {
        IPAddress sourceAddress;
        uint16_t sourcePort;

        IPAddress destinationAddress;
        uint16_t destinationPort;

        Protocol protocol;

        Direction direction;

        uint32_t bytes;

        pid_t pid{};
        uint32_t tid;
    };
}