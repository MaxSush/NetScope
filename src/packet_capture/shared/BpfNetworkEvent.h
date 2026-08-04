#pragma once

#include <cstdint>

namespace netscope
{
    struct BpfNetworkEvent
    {
        uint32_t pid;
        uint32_t tid;

		uint8_t type;
        uint8_t protocol;
        uint8_t family;
		uint8_t pad;

        union
        {
            struct
            {
                uint64_t bytes;
                uint8_t direction;
            } packet;

            struct
            {
                uint8_t closeReason;
            } close;
        } data;

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

        uint16_t source_port;
        uint16_t destination_port;
    };

    static_assert(sizeof(BpfNetworkEvent) == 72);
}