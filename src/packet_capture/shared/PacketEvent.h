#pragma once

#include <cstdint>

namespace netscope
{
    struct PacketEvent
    {
        uint32_t pid;
        uint32_t tid;

        uint64_t bytes;

        uint8_t protocol;
        uint8_t direction;
        uint8_t family;

        uint16_t source_port;
        uint16_t destination_port;

        union
        {
            uint32_t ipv4;
            uint8_t ipv6[16];
        } source;

        union
        {
            uint32_t ipv4;
            uint8_t ipv6[16];
        } destination;
    };

    static_assert(sizeof(PacketEvent) == 56);
}