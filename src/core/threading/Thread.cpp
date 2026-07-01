
#include "Thread.h"
#include <stdexcept>
#include "core/logger/Logger.h"

namespace netscope
{
	Thread::Thread(std::string name)
		:
		m_name(std::move(name)),
		m_isRunning(false),	
		m_stopRequested(false)
	{
	}

	Thread::~Thread()
	{
		if (m_thread.joinable())
		{
			LOG_WARN("Thread [{}] destroyed while still running", m_name);
			Join();
		}
	}

	void Thread::Start(std::function<void(Thread&)> function)
	{
		if (m_isRunning)
		{
			LOG_WARN("Thread [{}] already running", m_name);
			throw std::runtime_error("Thread already running.");
		}

		m_isRunning = true;
		m_stopRequested = false;

		LOG_INFO("Thread [{}] Started", m_name);

		m_thread = std::thread(
			[this, function]() {
				try
				{
					// Use ShouldStop() bool to stop
					function(*this);
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Thread {}: {}", m_name, e.what());
				}
				m_isRunning.store(false, std::memory_order_release);
				LOG_INFO("Thread [{}] Stopped", m_name);
			}
		);
	}

	void Thread::RequestStop()
	{
		m_stopRequested.store(true, std::memory_order_relaxed);
	}

	bool Thread::ShouldStop() const
	{
		return m_stopRequested.load(std::memory_order_relaxed);
	}

	void Thread::Join()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	bool Thread::IsRunning() const
	{
		return m_isRunning.load(std::memory_order_release);
	}

	const std::string& Thread::GetName() const
	{
		return m_name;
	}
}