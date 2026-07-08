#include "PacketDispatcher.h"

#include "INetworkEventProcessor.h"
#include <iostream>
#include <logger/Logger.h>

namespace netscope
{
	PacketDispatcher::PacketDispatcher(EventQueue<NetworkEvent>& queue)
		: m_queue(queue)
	{
		m_processors.reserve(8);
	}
	void PacketDispatcher::AddProcessor(INetworkEventProcessor& processor)
	{
		m_processors.push_back(&processor);
	}

	void PacketDispatcher::Run()
	{
		while (m_running)
		{
			try
			{
				NetworkEvent event = m_queue.Pop();

				for (auto* processor : m_processors)
				{
					processor->Process(event);
				}
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