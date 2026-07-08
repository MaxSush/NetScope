#include "CaptureRuntime.h"

namespace netscope
{
	CaptureRuntime::CaptureRuntime(EventQueue<NetworkEvent>& queue)
		: m_queue(queue)
	{
	}

	bool CaptureRuntime::Start()
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

		return true;
	}

	void CaptureRuntime::Stop()
	{
		m_running = false;

		if (m_pollThread.joinable())
		{
			m_pollThread.join();
		}

		m_loader.Unload();
	}
}
