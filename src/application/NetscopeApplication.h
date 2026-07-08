#pragma once
#include <thread>
#include <dispatcher/PacketDispatcher.h>
#include <manager/FlowManager.h>
#include <console/FlowReporter.h>
#include <runtime/CaptureRuntime.h>


namespace netscope
{
	class NetscopeApplication
	{
	public:
		NetscopeApplication();

		bool Start();
		void Stop();

	private:
		EventQueue<NetworkEvent> m_queue;
		CaptureRuntime m_capture;

		PacketDispatcher m_dispatcher;

		FlowManager m_flowManager;

		FlowReporter m_flowReporter;

		std::thread m_dispatcherThread;
		std::thread m_reportThread;

		bool m_running = false;
	};
}