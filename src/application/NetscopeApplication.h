#pragma once
#include <thread>
#include <dispatcher/PacketDispatcher.h>
#include <coordinator/NetworkCoordinator.h>
#include <manager/FlowManager.h>
#include <runtime/CaptureRuntime.h>
#include <ProcessManager.h>
#include <console/ConsoleReporter.h>

#include <lifecycle/LifecycleScheduler.h>

#include <persistence/PersistenceService.h>
#include <database/SQLiteDatabase.h>

namespace netscope
{
	class NetscopeApplication
	{
	public:
		NetscopeApplication();

		bool Start();
		void Stop();

		bool InitializeDatabase();

	private:
		EventQueue<NetworkEvent> m_queue;
		CaptureRuntime m_capture;

		processing::NetworkCoordinator m_coordinator;
		processing::ProcessingPipeline m_pipeline;
		processing::PacketDispatcher m_dispatcher;

		process::ProcessManager m_processManager;
		flow::FlowManager m_flowManager;

		ProcessAggregator m_processAggregator;
		presentation::ConsoleReporter m_consoleReporter;

		processing::LifecycleScheduler m_lifecycleScheduler;

		storage::SQLiteDatabase m_database;
		storage::PersistenceService m_persistenceService;

		std::thread m_captureRuntimeThread;
		std::thread m_persistenceServiceThread;
		std::thread m_dispatcherThread;
		std::thread m_reportThread;
		std::thread m_lifecycleThread;

		std::atomic<bool> m_running = false;
	};
}