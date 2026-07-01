#include "BpfLoader.h"
#include <bpf/libbpf.h>
#include <shared/PacketEvent.h>
#include <fmt/core.h>
#include <iostream>

namespace netscope
{
	static int HandleEvent(void* ctx, void* data, size_t size)
	{
		auto* queue = static_cast<EventQueue<NetworkEvent>*>(ctx);
		auto* packet = static_cast<PacketEvent*>(data);

		NetworkEvent event{};
		event.incomming = packet->direction == Direction::INCOMMING;
		event.pid = packet->pid;
		event.tid = packet->tid;
		event.bytes = packet->bytes;

		queue->Push(event);

		return 0;
	}

	bool BPFLoader::Load(EventQueue<NetworkEvent>& queue)
	{
		object = bpf_object__open_file("packet_monitor.bpf.o", nullptr);
		if (!object)
		{
			
			return false;
		}
		if (bpf_object__load(object))
		{
			return false;
		}
		bpf_program* send_program = bpf_object__find_program_by_name(object, "trace_send");
		if (!send_program)
		{
			return false;
		}
		sendLink = bpf_program__attach(send_program);
		if (!sendLink)
		{
			return false;
		}

		bpf_program* recv_program = bpf_object__find_program_by_name(object, "trace_recv");
		if (!recv_program)
		{
			return false;
		}
		recvLink = bpf_program__attach(recv_program);
		if (!recvLink)
		{
			return false;
		}

		bpf_map* map = bpf_object__find_map_by_name(object, "events");
		ringBuffer = ring_buffer__new(bpf_map__fd(map), HandleEvent, &queue, nullptr);

		return true;
	}

	void BPFLoader::Unload()
	{
		if (ringBuffer)
		{
			ring_buffer__free(ringBuffer);
			ringBuffer = nullptr;
		}

		if (recvLink)
		{
			bpf_link__destroy(recvLink);
			recvLink = nullptr;
		}

		if (sendLink)
		{
			bpf_link__destroy(sendLink);
			sendLink = nullptr;
		}

		if (object)
		{
			bpf_object__close(object);
			object = nullptr;
		}
	}

	void BPFLoader::Poll()
	{
		ring_buffer__poll(ringBuffer, 100);
	}
}
