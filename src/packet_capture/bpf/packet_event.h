#ifndef PACKET_EVENT_H
#define PACKET_EVENT_H

struct packet_event
{
    __u32 pid;
    __u32 tid;
    __u64 bytes;
    __u8 direction;
    __u8 padding[7];

};

#endif