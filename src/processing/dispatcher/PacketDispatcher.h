#pragma once
#include "shared/NetworkEvent.h"
#include <event/EventQueue.h>
#include <pipeline/ProcessingPipeline.h>

#include <atomic>

namespace netscope::processing
{
	class INetworkEventProcessor;

	class PacketDispatcher
	{
	public:
		explicit PacketDispatcher(EventQueue<NetworkEvent>& queue, ProcessingPipeline& pipeline);

		void Run();
		void Stop();

	private:
		EventQueue<NetworkEvent>& m_queue;

		std::atomic<bool> m_running = true;

		ProcessingPipeline& m_pipeline;
	};
}