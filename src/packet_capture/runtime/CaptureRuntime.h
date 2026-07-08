#pragma once

#include "loader/BpfLoader.h"

#include <thread>

namespace netscope
{
	class CaptureRuntime
	{
	public:
		CaptureRuntime(EventQueue<NetworkEvent>& queue);

		bool Start();
		void Stop();

	private:
		EventQueue<NetworkEvent>& m_queue;
		BPFLoader m_loader;

		std::thread m_pollThread;

		bool m_running = false;
	};
}