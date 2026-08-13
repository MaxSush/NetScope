#pragma once

#include "shared/NetworkEvent.h"
#include <event/EventQueue.h>


struct bpf_object;
struct ring_buffer;
struct bpf_link;
struct bpf_map;

namespace netscope
{
	class BPFLoader
	{
	public:
		bool Load(EventQueue<NetworkEvent>& queue);
		void Unload();

		void Poll();

		void TrackPid(uint32_t pid) const;
		void UntrackPid(uint32_t pid) const;

	private:
		bpf_object* object = nullptr;
		ring_buffer* ringBuffer = nullptr;

		bpf_link* sendLink = nullptr;
		bpf_link* recvLink = nullptr;
		bpf_link* stateLink = nullptr;

		bpf_link* udpSendLink = nullptr;
		bpf_link* udpRecvLink = nullptr;

		bpf_map* tracked_map = nullptr;
		bpf_link* processExitLink = nullptr;
	};
}