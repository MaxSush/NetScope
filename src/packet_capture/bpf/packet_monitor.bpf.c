#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_endian.h>

#include "packet_event.h"

#define DIRECTION_IN   0
#define DIRECTION_OUT  1

#define AF_INET   2
#define AF_INET6 10

struct
{
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 20);
} events SEC(".maps");


SEC("fentry/tcp_sendmsg")
int BPF_PROG(trace_send, struct sock *sk, struct msghdr *msg, size_t size, size_t copied, struct ubuf_info *uarg)
{
	u64 pid_tgid;
	u32 pid;
	struct packet_event* event;

	pid_tgid = bpf_get_current_pid_tgid();
	pid = pid_tgid >> 32;
	u32 tid = pid_tgid;

	event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
	if (!event)
	{
		return 0;
	}
	event->pid = pid;
	event->tid = tid;
	event->bytes = size;
	event->direction = DIRECTION_OUT;
	event->protocol = IPPROTO_TCP;
	event->family = BPF_CORE_READ(sk, __sk_common.skc_family);
	event->source_port = BPF_CORE_READ(sk, __sk_common.skc_num);
	event->destination_port = bpf_ntohs(BPF_CORE_READ(sk, __sk_common.skc_dport));

	if (event->family == AF_INET)
	{
		event->source.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_rcv_saddr);
		event->destination.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_daddr);
	}
	else
	{
		/* TODO: IPv6 */
	}

	bpf_ringbuf_submit(event, 0);

	return 0;
}

SEC("fexit/tcp_recvmsg")
int BPF_PROG(trace_recv, struct sock* sk, struct sk_buff* skb, int offset, struct msghdr* msg, int remaining_len, int ret)
{
	if (ret <= 0) {
		return 0;
	}

	u64 pid_tgid;
	u32 pid;

	struct packet_event* event;

	pid_tgid = bpf_get_current_pid_tgid();

	pid = pid_tgid >> 32;
	u32 tid = pid_tgid;

	event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);

	if (!event)
	{
		return 0;
	}

	event->pid = pid;
	event->tid = tid;
	event->direction = DIRECTION_IN;
	event->bytes = ret;
	event->protocol = IPPROTO_TCP;
	event->family = BPF_CORE_READ(sk, __sk_common.skc_family);
	event->source_port = BPF_CORE_READ(sk, __sk_common.skc_num);
	event->destination_port = bpf_ntohs(BPF_CORE_READ(sk, __sk_common.skc_dport));
	if (event->family == AF_INET)
	{
		event->source.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_rcv_saddr);
		event->destination.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_daddr);
	}
	else
	{
		/* TODO: IPv6 */
	}

	bpf_ringbuf_submit(event, 0);

	return 0;
}

char LICENSE[] SEC("license") = "GPL";

