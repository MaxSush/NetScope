#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-declarations"
#include "vmlinux.h"
#pragma clang diagnostic pop

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <bpf_hostname_event.h>

#ifndef TC_ACT_OK
#define TC_ACT_OK 0
#endif

#define CONN_AWAITING_HELLO		0
#define CONN_RESOLVED			1

struct conn_key
{
	__u8 family;
	__u8 pad[3];

	union {
		__u32 ipv4;
		__u8  ipv6[16];
	} src;
	union {
		__u32 ipv4;
		__u8  ipv6[16];
	} dst;

	__u16 src_port;
	__u16 dst_port;
};

struct
{
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 256 * 1024);
} hostname_events SEC(".maps");

struct
{
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__uint(max_entries, 4096);
	__type(key, struct conn_key);
	__type(value, __u32);
} tls_connections SEC(".maps");


/* TC ingress classifier: capture DNS responses (UDP src port 53) */
SEC("tc")
int tc_dns_ingress(struct __sk_buff* skb)
{
	void* data = (void*)(long)skb->data;
	void* data_end = (void*)(long)skb->data_end;

	// Parse Ethernet header
	struct ethhdr* eth = data;
	if ((void*)(eth + 1) > data_end)
	{
		return TC_ACT_OK; // TC_ACT_OK pass packet, dont drop it
	}

	__u16 eth_proto = bpf_ntohs(eth->h_proto);
	void* transport_hdr;
	__u8 family;


	// TODO: Handle VLAN tagged packets (0x8100) if needed
	if (eth_proto == 0x0800)	// IPv4
	{
		// Parse IP header
		struct iphdr* ip = (void*)(eth + 1);
		if ((void*)(ip + 1) > data_end)
		{
			return TC_ACT_OK;
		}

		// Only process UDP packets
		if (ip->protocol != IPPROTO_UDP)
		{
			return TC_ACT_OK;
		}

		// Calculate IP header length
		__u32 ip_hdr_len = ip->ihl * 4;
		if (ip_hdr_len < 20)
		{
			return TC_ACT_OK;
		}

		transport_hdr = (void*)((char*)ip + ip_hdr_len);
		family = FAMILY_IPV4;
	}
	else if (eth_proto == 0x86DD)
	{
		struct ipv6hdr* ip6 = (void*)(eth + 1);
		if ((void*)(ip6 + 1) > data_end)
		{
			return TC_ACT_OK;
		}

		if (ip6->nexthdr != IPPROTO_UDP)
		{
			return TC_ACT_OK;
		}

		transport_hdr = (void*)(ip6 + 1);
		family = FAMILY_IPV6;
	}
	else
	{
		return TC_ACT_OK;
	}
	
	// Parse UDP header
	struct udphdr* udp = transport_hdr;
	if ((void*)(udp + 1) > data_end)
	{
		return TC_ACT_OK;
	}

	// Only process DNS packets (UDP port 53)
	if (bpf_ntohs(udp->source) != 53)
	{
		return TC_ACT_OK;
	}

	// Calculate DNS payload length
	void* dns_data = (void*)(udp + 1);
	__u32 dns_len = (char*)data_end - (char*)dns_data;
	if (dns_len < 12 || dns_len > MAX_PAYLOAD_LEN)
	{
		return TC_ACT_OK;
	}

	// Reserve space in the ring buffer for the event
	struct bpf_hostname_event *event;
	event = bpf_ringbuf_reserve(&hostname_events, sizeof(*event), 0);
	if (!event)
	{
		return TC_ACT_OK;
	}

	// Fill in the event structure
	event->event_type = HOSTNAME_EVENT_DNS;
	event->family = family;
	event->src_port = bpf_ntohs(udp->source);
	event->dst_port = bpf_ntohs(udp->dest);

	if (family == FAMILY_IPV4)
	{
		struct iphdr* ip = (void*)(eth + 1);
		event->src_ip.ipv4 = ip->saddr;
		event->dst_ip.ipv4 = ip->daddr;
	}
	else
	{
		struct ipv6hdr* ip6 = (void*)(eth + 1);
		__builtin_memcpy(&event->src_ip.ipv6, &ip6->saddr, 16);
		__builtin_memcpy(&event->dst_ip.ipv6, &ip6->daddr, 16);
	}

	dns_len &= (MAX_PAYLOAD_LEN - 1);
	if (dns_len < 12)
	{
		bpf_ringbuf_discard(event, 0);
		return TC_ACT_OK;
	}

	event->payload_len = dns_len;

	// Copy the DNS payload into the event structure
	__u32 offset = (char*)dns_data - (char*)data;
	if (bpf_skb_load_bytes(skb, offset, event->payload, dns_len) < 0)
	{
		bpf_ringbuf_discard(event, 0);
		return TC_ACT_OK;
	}

	// Submit the event to the ring buffer
	bpf_ringbuf_submit(event, 0);
	return TC_ACT_OK;
}

/* TC egress classifier: capture TLS Client Hello (TCP dst port 443) */
SEC("tc")
int tc_sni_egress(struct __sk_buff* skb)
{
	void* data = (void*)(long)skb->data;
	void* data_end = (void*)(long)skb->data_end;

	// Parse Ethernet header
	struct ethhdr* eth = data;
	if ((void*)(eth + 1) > data_end)
	{
		return TC_ACT_OK;
	}

	__u16 eth_proto = bpf_ntohs(eth->h_proto);
	void* transport_hdr;
	__u8 family;

	if (eth_proto == 0x0800)	// IPv4
	{
		// Parse IP header
		struct iphdr* ip = (void*)(eth + 1);
		if ((void*)(ip + 1) > data_end)
		{
			return TC_ACT_OK;
		}

		// Only process TCP packets
		if (ip->protocol != IPPROTO_TCP)
		{
			return TC_ACT_OK;
		}

		// Calculate IP header length
		__u32 ip_hdr_len = ip->ihl * 4;
		if (ip_hdr_len < 20)
		{
			return TC_ACT_OK;
		}

		transport_hdr = (void*)((char*)ip + ip_hdr_len);
		family = FAMILY_IPV4;
	}
	else if (eth_proto == 0x86DD)
	{
		struct ipv6hdr* ip6 = (void*)(eth + 1);
		if ((void*)(ip6 + 1) > data_end)
		{
			return TC_ACT_OK;
		}

		if (ip6->nexthdr != IPPROTO_TCP)
		{
			return TC_ACT_OK;
		}

		transport_hdr = (void*)(ip6 + 1);
		family = FAMILY_IPV6;
	}
	else
	{
		return TC_ACT_OK;
	}

	// Parse TCP header
	struct tcphdr* tcp = transport_hdr;
	if ((void*)(tcp + 1) > data_end)
	{
		return TC_ACT_OK;
	}

	// Only process packets destined for port 443 (HTTPS)
	if (bpf_ntohs(tcp->dest) != 443)
	{
		return TC_ACT_OK;
	}

	// Create a connection key for tracking TLS connections
	struct conn_key key = {};
	key.family = family;
	key.src_port = bpf_ntohs(tcp->source);
	key.dst_port = 443;

	if (family == FAMILY_IPV4)
	{
		struct iphdr* ip = (void*)(eth + 1);
		key.src.ipv4 = ip->saddr;
		key.dst.ipv4 = ip->daddr;
	}
	else
	{
		struct ipv6hdr* ip6 = (void*)(eth + 1);
		__builtin_memcpy(&key.src.ipv6, &ip6->saddr, 16);
		__builtin_memcpy(&key.dst.ipv6, &ip6->daddr, 16);
	}

	// If this is a SYN packet (start of a new connection), add it to the map with state CONN_AWAITING_HELLO
	if (tcp->syn && !tcp->ack)
	{
		__u32 state = CONN_AWAITING_HELLO;
		bpf_map_update_elem(&tls_connections, &key, &state, BPF_ANY);
		return TC_ACT_OK;
	}

	// If this is not a SYN packet, check if the connection is in the map and if it is awaiting a TLS Client Hello
	__u32* state = bpf_map_lookup_elem(&tls_connections, &key);
	if (!state || *state == CONN_RESOLVED)
	{
		return TC_ACT_OK;
	}

	// If the connection is awaiting a TLS Client Hello, check if the payload contains a TLS Client Hello message
	__u32 tcp_hdr_len = tcp->doff * 4;
	void* payload = (void*)((char*)tcp + tcp_hdr_len);
	__u32 payload_len = (char*)data_end - (char*)payload;

	// If the payload is less than 6 bytes, it cannot contain a TLS Client Hello message
	if (payload_len < 6)
	{
		return TC_ACT_OK;
	}

	// Read first bytes to check for TLS Client Hello signature
	__u8 tls_header[6];
	__u32 payload_offset = (char*)payload - (char*)data;
	if (bpf_skb_load_bytes(skb, payload_offset, tls_header, 6) < 0)
	{
		return TC_ACT_OK;
	}

	// TLS Client Hello message starts with 0x16 (Handshake), followed by Handshake Type 0x01 (TLS version 1.0)
	if (tls_header[0] != 0x16 || tls_header[5] != 0x01)
	{
		return TC_ACT_OK;
	}

	// If we reach here, we have a TLS Client Hello message. Capture the payload and emit an event.
	__u32 copy_len = payload_len;

	// Same verifier fix as tc_dns_ingress: bitmask ensures [0, 511]
	copy_len &= (MAX_PAYLOAD_LEN - 1);
	if (!copy_len)
	{
		return TC_ACT_OK;
	}

	// Reserve space in the ring buffer for the event
	struct bpf_hostname_event* event;
	event = bpf_ringbuf_reserve(&hostname_events, sizeof(*event), 0);
	if (!event)
	{
		return TC_ACT_OK;
	}

	// Fill in the event structure
	event->event_type = HOSTNAME_EVENT_TLS;
	event->family = family;
	event->src_port = bpf_ntohs(tcp->source);
	event->dst_port = 443;
	event->payload_len = copy_len;

	if (family == FAMILY_IPV4)
	{
		struct iphdr* ip = (void*)(eth + 1);
		event->src_ip.ipv4 = ip->saddr;
		event->dst_ip.ipv4 = ip->daddr;
	}
	else
	{
		struct ipv6hdr* ip6 = (void*)(eth + 1);
		__builtin_memcpy(&event->src_ip.ipv6, &ip6->saddr, 16);
		__builtin_memcpy(&event->dst_ip.ipv6, &ip6->daddr, 16);
	}

	// Copy the TLS Client Hello payload into the event structure
	if (bpf_skb_load_bytes(skb, payload_offset, event->payload, copy_len) < 0)
	{
		bpf_ringbuf_discard(event, 0);
		return TC_ACT_OK;
	}

	// Submit the event to the ring buffer
	bpf_ringbuf_submit(event, 0);

	// Update the connection state to CONN_RESOLVED
	__u32 resolved = CONN_RESOLVED;
	bpf_map_update_elem(&tls_connections, &key, &resolved, BPF_ANY);
	return TC_ACT_OK;
}

char LICENSE[] SEC("license") = "GPL";