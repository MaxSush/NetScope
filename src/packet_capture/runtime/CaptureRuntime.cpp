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

	void netscope::CaptureRuntime::TrackPid(uint32_t pid) const
	{
		m_loader.TrackPid(pid);
	}

	void netscope::CaptureRuntime::UntrackPid(uint32_t pid) const
	{
		m_loader.UntrackPid(pid);
	}
}
