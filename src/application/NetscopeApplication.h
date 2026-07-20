#pragma once
#include <thread>
#include <dispatcher/PacketDispatcher.h>
#include <coordinator/NetworkCoordinator.h>
#include <manager/FlowManager.h>
#include <runtime/CaptureRuntime.h>
#include <DebugSubscriber.h>
#include <ProcessManager.h>
#include <console/ConsoleReporter.h>


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

		processing::NetworkCoordinator m_coordinator;
		processing::ProcessingPipeline m_pipeline;
		processing::PacketDispatcher m_dispatcher;

		DebugSubscriber m_debug;

		process::ProcessManager m_processManager;
		flow::FlowManager m_flowManager;

		ProcessAggregator m_processAggregator;
		presentation::ConsoleReporter m_consoleReporter;

		std::thread m_dispatcherThread;
		std::thread m_reportThread;

		bool m_running = false;
	};
}