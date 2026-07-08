#include "NetscopeApplication.h"
#include <logger/Logger.h>


namespace netscope
{
	NetscopeApplication::NetscopeApplication()
		:
		m_capture(m_queue),
		m_dispatcher(m_queue),
		m_flowReporter(m_flowManager)
	{
		m_dispatcher.AddProcessor(m_flowManager);
	}

	bool NetscopeApplication::Start()
	{
		if (m_capture.Start())
		{
			m_running = true;
		}
		else
		{
			LOG_ERROR("Error starting Capture Runtime");
			return false;
		}

		m_dispatcherThread = std::thread([this]()
			{
				m_dispatcher.Run();
			});

		m_reportThread = std::thread([this]()
			{
				while (m_running)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));

					m_flowReporter.Report();
				}
			});

		return true;
	}

	void NetscopeApplication::Stop()
	{
		m_running = false;

		m_capture.Stop();

		m_queue.Shutdown();

		m_dispatcher.Stop();

		if (m_dispatcherThread.joinable())
		{
			m_dispatcherThread.join();
		}

		if (m_reportThread.joinable())
		{
			m_reportThread.join();
		}

	}

}