#pragma once

#include "loader/BpfLoader.h"
#include "IBpfTracker.h"

#include <thread>

namespace netscope
{
	class CaptureRuntime : public IBpfTracker
	{
	public:
		CaptureRuntime(EventQueue<NetworkEvent>& queue);

		bool Initialize();
		void Run();
		void Stop();

		// Inherited via IBpfTracker
		void TrackPid(uint32_t pid) const override;
		void UntrackPid(uint32_t pid) const override;

	private:
		EventQueue<NetworkEvent>& m_queue;
		BPFLoader m_loader;

	};
}