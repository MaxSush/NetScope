#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-declarations"
#include "vmlinux.h"
#pragma clang diagnostic pop

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_endian.h>

#include "bpf_network_event.h"

#define PACKET				0
#define CONNECTION_CLOSE	1
#define PROCESS_EXIT		2

#define DIRECTION_IN		0
#define DIRECTION_OUT		1

#define AF_INET				2
#define AF_INET6			10

struct
{
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 20);
} events SEC(".maps");

struct
{
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(max_entries, 1024);
	__type(key, __u32);
	__type(value, __u8);
} tracked_pids SEC(".maps");

static __always_inline int emit_packet_event(struct sock* sk, __u8 protocol, __u64 bytes, __u8 direction)
{
	u64 pid_tgid = bpf_get_current_pid_tgid();
	__u32 pid = pid_tgid >> 32;
	__u32 tid = (__u32)pid_tgid;

	struct bpf_network_event* event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
	if (!event)
	{
		return 0;
	}

	event->pid = pid;
	event->tid = tid;

	event->type = PACKET;
	event->protocol = IPPROTO_TCP;
	event->family = BPF_CORE_READ(sk, __sk_common.skc_family);


	event->data.packet.bytes = bytes;
	event->data.packet.direction = DIRECTION_OUT;

	if (event->family == AF_INET)
	{
		event->source.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_rcv_saddr);
		event->destination.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_daddr);
	}
	else
	{
		/* TODO: IPv6 */
	}

	event->source_port = BPF_CORE_READ(sk, __sk_common.skc_num);
	event->destination_port = bpf_ntohs(BPF_CORE_READ(sk, __sk_common.skc_dport));

	bpf_ringbuf_submit(event, 0);

	return 0;
}


SEC("fentry/tcp_sendmsg")
int BPF_PROG(trace_send, struct sock *sk, struct msghdr *msg, size_t size, size_t copied, struct ubuf_info *uarg)
{
	return emit_packet_event(sk, IPPROTO_TCP, size, DIRECTION_OUT);
}

SEC("fexit/tcp_recvmsg")
int BPF_PROG(trace_recv, struct sock* sk, struct sk_buff* skb, int offset, struct msghdr* msg, int remaining_len, int ret)
{
	if (ret <= 0) {
		return 0;
	}

	return emit_packet_event(sk, IPPROTO_TCP, (__u64)ret, DIRECTION_IN);
}

SEC("fentry/tcp_set_state")
int BPF_PROG(trace_set_state, struct sock* sk, int state)
{
	if (state != TCP_CLOSE)
	{
		return 0;
	}

	struct bpf_network_event* event;

	event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
	if (!event)
	{
		return 0;
	}

	event->type = CONNECTION_CLOSE;
	event->protocol = BPF_CORE_READ(sk, sk_protocol);
	event->family = BPF_CORE_READ(sk, __sk_common.skc_family);

	if (event->family == AF_INET)
	{
		event->source.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_rcv_saddr);
		event->destination.ipv4 = BPF_CORE_READ(sk, __sk_common.skc_daddr);
	}
	else
	{
		/* TODO: IPv6 */
	}

	event->source_port = BPF_CORE_READ(sk, __sk_common.skc_num);
	event->destination_port = bpf_ntohs(BPF_CORE_READ(sk, __sk_common.skc_dport));

	int err = BPF_CORE_READ(sk, sk_err);
	__u8 shutdown = BPF_CORE_READ(sk, sk_shutdown);

	event->data.close.err = err;
	event->data.close.shutdown_mask = shutdown;

	bpf_printk("flow: %d:%d => %d:%d state=%d err=%d mask=%d", event->source.ipv4, event->source_port, event->destination.ipv4, event->destination_port, state, err, shutdown);

	bpf_ringbuf_submit(event, 0);
	return 0;
}

SEC("tp_btf/sched_process_exit")
int BPF_PROG(trace_process_exit, struct task_struct* task)
{
	__u32 pid = BPF_CORE_READ(task, tgid);
	__u32 tid = BPF_CORE_READ(task, pid);

	if (pid != tid)
	{
		return 0;
	}

	__u8* tracked = bpf_map_lookup_elem(&tracked_pids, &pid);
	if (!tracked)
	{
		return 0;
	}

	struct bpf_network_event* event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
	if (!event)
	{
		return 0;
	}

	event->type = PROCESS_EXIT;
	event->pid = pid;
	event->tid = tid;

	event->protocol = 0;
	event->family = 0;
	event->source_port = 0;
	event->destination_port = 0;

	bpf_ringbuf_submit(event, 0);

	bpf_map_delete_elem(&tracked_pids, &pid);

	return 0;
}

char LICENSE[] SEC("license") = "GPL";