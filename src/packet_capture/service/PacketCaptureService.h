#pragma once

#include "loader/BpfLoader.h"
#include "dispatcher/PacketDispatcher.h"
#include "statistics/StatisticsAggregator.h"
#include "ProcessCache.h"
#include "StatisticsReporter.h"
#include <manager/FlowManager.h>

#include <thread>
#include "FlowReporter.h"

namespace netscope
{
	class PacketCaptureService
	{
	public:
		PacketCaptureService();

		bool Start();
		void Stop();


	private:
		EventQueue<NetworkEvent> m_queue;
		BPFLoader m_loader;

		PacketDispatcher m_dispatcher;

		StatisticsAggregator m_aggregator;
		FlowManager m_flowManager;

		FlowReporter m_flowReporter;

		ProcessCache m_processCache;
		ProcessManager m_processManager;
		StatisticsReporter m_reporter;

		std::thread m_pollThread;
		std::thread m_dispatcherThread;
		std::thread m_reportThread;

		bool m_running = false;

	};
}