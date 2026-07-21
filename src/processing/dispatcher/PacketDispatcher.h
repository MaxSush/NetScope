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

	private:
		EventQueue<NetworkEvent>& m_queue;

		ProcessingPipeline& m_pipeline;
	};
}