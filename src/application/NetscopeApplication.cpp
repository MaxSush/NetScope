#include "NetscopeApplication.h"
#include <logger/Logger.h>
#include <Config.h>
#include <schema/SchemaManager.h>

namespace netscope
{
	NetscopeApplication::NetscopeApplication()
		:
		m_capture(m_queue),
		m_dispatcher(m_queue, m_pipeline),
		m_pipeline(m_coordinator),
		m_processAggregator(m_processManager),
		m_consoleReporter(m_processAggregator, m_flowManager),
		m_persistenceService(m_database),
		m_lifecycleScheduler(m_coordinator)
	{
		m_pipeline.AddStage(m_flowManager);
		m_pipeline.AddStage(m_processManager);

		m_lifecycleScheduler.AddProcessor(m_flowManager);
		//m_lifecycleScheduler.AddProcessor(m_processManager);

		m_coordinator.Subscribe(m_persistenceService);
	}

	bool NetscopeApplication::Start()
	{
		if (!InitializeDatabase())
		{
			return false;
		}

		if (!m_capture.Initialize())
		{
			LOG_ERROR("Error starting Capture Runtime");
			return false;
		}
		m_running = true;

		m_persistenceServiceThread = std::thread([this]()
			{
				while (m_running)
				{
					m_persistenceService.Run();
				}
			});

		m_dispatcherThread = std::thread([this]()
			{
				while (m_running)
				{
					LOG_TRACE("LifecycleScheduler tick");

					m_dispatcher.Run();
				}
			});

		m_lifecycleThread = std::thread([this]()
			{
				while (m_running)
				{
					std::this_thread::sleep_for(config::LIFECYCLE_SCHEDULER_INTERVAL);
					
					if (!m_running)
					{
						break;
					}
					
					LOG_TRACE("LifecycleScheduler tick");

					m_lifecycleScheduler.Process();
				}
			});

		m_reportThread = std::thread([this]()
			{
				while (m_running)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));

					//m_processCache.CleanUp(FLOW_IDLE_TIMEOUT);

					m_consoleReporter.Report();

					//size_t removed = m_flowManager.RemoveIdleFlows(FLOW_IDLE_TIMEOUT);
					//LOG_WARN("Removed Idle Connections: {}", removed);
				}
			});

		m_captureRuntimeThread = std::thread([this]() 
			{
				while (m_running)
				{
					m_capture.Run();
				}
			});

		return true;
	}

	void NetscopeApplication::Stop()
	{
		m_running = false;

		m_capture.Stop();

		m_queue.Shutdown();
		m_persistenceService.Stop();

		if(m_captureRuntimeThread.joinable())
		{
			m_captureRuntimeThread.join();
		}

		if (m_persistenceServiceThread.joinable())
		{
			m_persistenceServiceThread.join();
		}

		if (m_dispatcherThread.joinable())
		{
			m_dispatcherThread.join();
		}

		if (m_reportThread.joinable())
		{
			m_reportThread.join();
		}

	}
	bool netscope::NetscopeApplication::InitializeDatabase()
	{
		auto res = m_database.Open("netscope.db");
		if (!res)
		{
			LOG_ERROR(res.message);
			return false;
		}

		storage::SchemaManager schema;

		res = schema.Initialize(m_database);
		if (!res)
		{
			LOG_ERROR(res.message);
			return false;
		}
		return true;
	}
}