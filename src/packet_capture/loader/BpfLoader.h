#pragma once

#include <shared/NetworkEvent.h>
#include <core/event/EventQueue.h>

struct bpf_object;
struct ring_buffer;
struct bpf_link;

namespace netscope
{
	class BPFLoader
	{
	public:
		bool Load(EventQueue<NetworkEvent>& queue);
		void Unload();

		void Poll();

	private:
		bpf_object* object = nullptr;
		ring_buffer* ringBuffer = nullptr;
		bpf_link* sendLink = nullptr;
		bpf_link* recvLink = nullptr;

	};
}