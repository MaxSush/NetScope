#ifndef BPF_NETWORK_EVENT_H
#define BPF_NETWORK_EVENT_H

struct bpf_network_event
{
    __u32 pid;
    __u32 tid;

    __u8 type;
    __u8 protocol;
    __u8 family;
    __u8 pad;

    union
    {
        struct
        {
            __u64 bytes;
            __u8 direction;
        } packet;

        struct
        {
			__u8 close_reason;
            __u8 shutdown_mask;
            __s32 err;
        } close;
    } data;

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

    __u16 source_port;
    __u16 destination_port;

    __u32 ifindex;
};

#endif