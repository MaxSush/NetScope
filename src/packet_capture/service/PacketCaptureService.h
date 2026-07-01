#pragma once

#include <core/event/EventQueue.h>
#include <shared/NetworkEvent.h>
#include <statistics/NetworkAggregator.h>
#include "StatisticsReporter.h"
#include <loader/BpfLoader.h>
#include <thread>

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

		NetworkAggregator m_aggregator;
		ProcessCache m_processCache;
		ProcessManager m_processManager;

		StatisticsReporter m_reporter;

		std::thread m_pollThread;
		std::thread m_aggregatorThread;
		std::thread m_reportThread;

		bool m_running = false;

	};
}