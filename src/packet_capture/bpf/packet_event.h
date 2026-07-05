#ifndef PACKET_EVENT_H
#define PACKET_EVENT_H

struct packet_event
{
    __u32 pid;
    __u32 tid;

    __u64 bytes;

    __u8 protocol;
    __u8 direction;
    __u8 family;

    __u16 source_port;
    __u16 destination_port;

    union
    {
        __u32 ipv4;
        __u8 ipv6[16];
    } source;

    union
    {
        __u32 ipv4;
        __u8 ipv6[16];
    } destination;
};

#endif