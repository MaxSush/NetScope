#pragma once
#include <thread>
#include <dispatcher/PacketDispatcher.h>
#include <manager/FlowManager.h>
#include <console/FlowReporter.h>
#include <runtime/CaptureRuntime.h>
#include <console/ProcessReporter.h>
#include <event/DomainEventBus.h>
#include <processor/FlowProcessor.h>


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

		process::ProcessManager m_processManager;
		process::ProcessCache m_processCache;

		ProcessAggregator m_processAggregator;
		flow::FlowManager m_flowManager;

		core::DomainEventBus<processing::FlowUpdatedEvent> m_eventBus;
		processing::FlowProcessor m_flowProcessor;

		FlowReporter m_flowReporter;
		ProcessReporter m_processReporter;

		std::thread m_dispatcherThread;
		std::thread m_reportThread;

		bool m_running = false;
	};
}