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
		while (m_running)
		{
			try
			{
				NetworkEvent event = m_queue.Pop();

				m_pipeline.Process(event);
			}
			catch (...)
			{
				LOG_ERROR("PacketDispatcher::Run Unkown Error occured");
				break;
			}
		}
	}

	void PacketDispatcher::Stop()
	{
		m_running = false;
	}

}