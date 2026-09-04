#include "NetscopeApplication.h"
#include <logger/Logger.h>
#include <schema/SchemaManager.h>

namespace netscope
{
	NetscopeApplication::NetscopeApplication()
		:
		m_capture(m_queue),
		m_dispatcher(m_queue, m_pipeline),
		m_pipeline(m_coordinator),
		m_processManager(m_capture),
		m_processAggregator(m_processManager),
		m_consoleReporter(m_processAggregator, m_flowManager),
		m_persistenceService(m_database),
		m_lifecycleScheduler(m_coordinator)
	{
		m_pipeline.AddStage(m_processManager);
		m_pipeline.AddStage(m_flowManager);

		m_lifecycleScheduler.AddProcessor(m_processManager);
		m_lifecycleScheduler.AddProcessor(m_flowManager);

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
					LOG_TRACE("Dispatcher tick");

					m_dispatcher.Run();
				}
			});

		m_lifecycleThread = std::thread([this]()
			{
				while (m_running)
				{
					std::this_thread::sleep_for(Config::Get().lifecycleInterval);
					
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
					std::this_thread::sleep_for(Config::Get().reportInterval);

					m_consoleReporter.Report();
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

		if (m_lifecycleThread.joinable())
		{
			m_lifecycleThread.join();
		}

	}

	bool netscope::NetscopeApplication::InitializeDatabase()
	{
		auto res = m_database.Open(Config::Get().databasePath);
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