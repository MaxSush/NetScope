#include "PacketCaptureService.h"
#include <iostream>

namespace netscope
{
	PacketCaptureService::PacketCaptureService()
		:
		m_aggregator(m_queue),
		m_processCache(m_processManager),
		m_reporter(m_aggregator, m_processCache)
	{
	}

	bool PacketCaptureService::Start()
	{
		if (!m_loader.Load(m_queue))
		{
			return false;
		}
		m_running = true;

		m_pollThread = std::thread([this]()
			{
				while (m_running)
				{
					m_loader.Poll();
				}
			});

		m_aggregatorThread = std::thread([this]()
			{
				m_aggregator.Run();
			});

		m_reportThread = std::thread([this]()
			{
				while (m_running)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));

					m_reporter.Report();
				}
			});

		return true;
	}

	void PacketCaptureService::Stop()
	{
		m_running = false;

		m_queue.Shutdown();

		m_aggregator.Stop();

		if (m_pollThread.joinable())
		{
			m_pollThread.join();
		}

		if (m_aggregatorThread.joinable())
		{
			m_aggregatorThread.join();
		}

		if (m_reportThread.joinable())
		{
			m_reportThread.join();
		}

		m_loader.Unload();
	}
}

