#ifndef BPF_HOSTNAME_EVENT_H
#define BPF_HOSTNAME_EVENT_H

#define HOSTNAME_EVENT_DNS		0
#define HOSTNAME_EVENT_TLS		1
#define MAX_PAYLOAD_LEN			512

#define FAMILY_IPV4				2
#define FAMILY_IPV6				10

struct bpf_hostname_event
{
	__u8 event_type;
	__u8 family;
	__u8 pad[2];

	union
	{
		__u32 ipv4;
		__u8 ipv6[16];
	} src_ip;

	union
	{
		__u32 ipv4;
		__u8 ipv6[16];
	} dst_ip;

	__u16 src_port;
	__u16 dst_port;

	__u32 payload_len;
	__u8 payload[MAX_PAYLOAD_LEN];
};


#endif // !BPF_HOSTNAME_EVENT_H