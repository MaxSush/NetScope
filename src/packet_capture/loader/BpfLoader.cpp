#include "BpfLoader.h"

#include "shared/BpfNetworkEvent.h"
#include "shared/NetworkEvent.h"

#include <bpf/libbpf.h>


namespace netscope
{
	static int HandleEvent(void* ctx, void* data, size_t size)
	{
		auto* queue = static_cast<EventQueue<NetworkEvent>*>(ctx);
		auto* packet = static_cast<BpfNetworkEvent*>(data);

		NetworkEvent event{};

		event.pid = packet->pid;
		event.tid = packet->tid;

		event.protocol = flow::ToProtocol(packet->protocol);
		event.family = flow::AddressFamily::IPv4;

		event.sourcePort = packet->source_port;
		event.destinationPort = packet->destination_port;
		
		if (packet->type == 0)  // Packet
		{
			event.type = NetworkEventType::PACKET;
			event.bytes = packet->data.packet.bytes;
			event.direction = (packet->data.packet.direction == 0) ? Direction::INCOMMING : Direction::OUTGOING;
		}
		else if (packet->type == 1)  // Connection closed
		{
			event.type = NetworkEventType::CONNECTION_CLOSED;

			int err = packet->data.close.err;
			int shutdown_mask = packet->data.close.shutdownMask;
			event.close.kernelError = err;
			event.close.shutdownMask = shutdown_mask;

			if (err == 111)
			{
				event.close.closeReason = CloseReason::CONNECTION_REFUSED;
			}
			else if (err == 104)
			{
				event.close.closeReason = CloseReason::TCP_RESET;
			} 
			else if (err == 110)
			{
				event.close.closeReason = CloseReason::TCP_TIMEOUT;
			}
			else if (err != 0)
			{
				event.close.closeReason = CloseReason::KERNEL_ERROR;
			}
			else if (shutdown_mask & 3)
			{
				event.close.closeReason = CloseReason::TCP_FIN;
			}
			else
			{
				event.close.closeReason = CloseReason::UNKNOWN;
			}
		}
		else if (packet->type == 2) //Process Exit
		{
			event.type = NetworkEventType::PROCESS_EXITED;
			event.close.closeReason = CloseReason::PROCESS_EXITED;
		}

		if (event.family == flow::AddressFamily::IPv4)
		{
			event.sourceAddress = flow::IPAddress::IPv4(packet->source.ipv4);
			event.destinationAddress = flow::IPAddress::IPv4(packet->destination.ipv4);
		}
		else
		{
			//event.sourceAddress = IPAddress::IPv6(packet->source.ipv6);
			//event.destinationAddress = IPAddress::IPv6(packet->destination.ipv6);
		}
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

		bpf_program* tcp_state_program = bpf_object__find_program_by_name(object, "trace_set_state");
		if (!tcp_state_program)
		{
			return false;
		}
		stateLink = bpf_program__attach(tcp_state_program);
		if (!stateLink)
		{
			return false;
		}

		bpf_program* process_exit_program = bpf_object__find_program_by_name(object, "trace_process_exit");
		if (!process_exit_program)
		{
			return false;
		}
		processExitLink = bpf_program__attach(process_exit_program);
		if (!processExitLink)
		{
			return false;
		}

		bpf_program* udp_send_program = bpf_object__find_program_by_name(object, "trace_udp_send");
		if (!udp_send_program)
		{
			return false;
		}
		udpSendLink = bpf_program__attach(udp_send_program);
		if (!udpSendLink)
		{
			return false;
		}

		bpf_program* udp_recv_program = bpf_object__find_program_by_name(object, "trace_udp_recv");
		if (!udp_recv_program)
		{
			return false;
		}
		udpRecvLink = bpf_program__attach(udp_recv_program);
		if (!udpRecvLink)
		{
			return false;
		}

		tracked_map = bpf_object__find_map_by_name(object, "tracked_pids");


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

		if (stateLink)
		{
			bpf_link__destroy(stateLink);
			stateLink = nullptr;
		}

		if (processExitLink)
		{
			bpf_link__destroy(processExitLink);
			processExitLink = nullptr;
		}

		if (udpSendLink)
		{
			bpf_link__destroy(udpSendLink);
			udpSendLink = nullptr;
		}

		if (udpRecvLink)
		{
			bpf_link__destroy(udpRecvLink);
			udpRecvLink = nullptr;
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

	void netscope::BPFLoader::TrackPid(uint32_t pid) const
	{
		uint8_t val = 1;
		bpf_map__update_elem(tracked_map, &pid, sizeof(pid), &val, sizeof(val), BPF_ANY);
	}

	void netscope::BPFLoader::UntrackPid(uint32_t pid) const
	{
		bpf_map__delete_elem(tracked_map, &pid, sizeof(pid), BPF_ANY);
	}

}
