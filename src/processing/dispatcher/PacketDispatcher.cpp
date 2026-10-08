#include "PacketDispatcher.h"

#include "INetworkEventProcessor.h"
#include <iostream>
#include <logger/Logger.h>

namespace netscope::processing
{
	PacketDispatcher::PacketDispatcher(EventQueue<NetworkEvent>& queue, ProcessingPipeline& pipeline)
		: m_queue(queue),
		m_pipeline(pipeline)
	{
	}

	void PacketDispatcher::Run()
	{
		try
		{
			NetworkEvent event = m_queue.Pop();
			m_pipeline.Process(event);
		}
		catch (const std::runtime_error&)
		{
			// Queue shut down, silently exit
			return;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("PacketDispatcher::Run Error: {}", e.what());
		}
		catch (...)
		{
			LOG_ERROR("PacketDispatcher::Run Unknown Error occurred");
		}
	}
}