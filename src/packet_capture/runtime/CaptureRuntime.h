#pragma once

#include "loader/BpfLoader.h"

#include <thread>

namespace netscope
{
	class CaptureRuntime
	{
	public:
		CaptureRuntime(EventQueue<NetworkEvent>& queue);

		bool Initialize();
		void Run();
		void Stop();

	private:
		EventQueue<NetworkEvent>& m_queue;
		BPFLoader m_loader;

	};
}