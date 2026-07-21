#include "CaptureRuntime.h"

namespace netscope
{
	CaptureRuntime::CaptureRuntime(EventQueue<NetworkEvent>& queue)
		: m_queue(queue)
	{
	}

	bool CaptureRuntime::Initialize()
	{
		return m_loader.Load(m_queue);
	}

	void CaptureRuntime::Run()
	{
		m_loader.Poll();
	}

	void CaptureRuntime::Stop()
	{
		m_loader.Unload();
	}
}
