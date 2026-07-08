#pragma once
#include <thread>
#include <dispatcher/PacketDispatcher.h>
#include <manager/FlowManager.h>
#include <console/FlowReporter.h>
#include <runtime/CaptureRuntime.h>
#include <console/ProcessReporter.h>



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

		ProcessManager m_processManager;
		ProcessCache m_processCache;

		ProcessAggregator m_processAggregator;
		FlowManager m_flowManager;

		FlowReporter m_flowReporter;
		ProcessReporter m_processReporter;

		std::thread m_dispatcherThread;
		std::thread m_reportThread;

		bool m_running = false;
	};
}