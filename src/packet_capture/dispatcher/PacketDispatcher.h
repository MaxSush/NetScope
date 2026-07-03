#pragma once
#include "shared/NetworkEvent.h"
#include <event/EventQueue.h>

#include <atomic>

namespace netscope
{
	class INetworkEventProcessor;

	class PacketDispatcher
	{
	public:
		explicit PacketDispatcher(EventQueue<NetworkEvent>& queue);

		void AddProcessor(INetworkEventProcessor& processor);

		void Run();
		void Stop();

	private:
		EventQueue<NetworkEvent>& m_queue;

		std::atomic<bool> m_running = true;

		std::vector<INetworkEventProcessor*> m_processors;
	};
}