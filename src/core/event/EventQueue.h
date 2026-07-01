#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>


namespace netscope
{
	template<typename T>
	class EventQueue
	{
	public:
		void Push(const T& item);
		T Pop();
		bool Empty() const;
		size_t Size() const;
		void Shutdown();

	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
		bool m_shutdown = false;
		std::condition_variable m_condition;
	};

	template<typename T>
	inline void EventQueue<T>::Push(const T& item)
	{
		{
			std::lock_guard lock(m_mutex);
			m_queue.push(item);
		}
		m_condition.notify_one();
	}

	template<typename T>
	inline T EventQueue<T>::Pop()
	{
		std::unique_lock lock(m_mutex);

		m_condition.wait(lock, [this]()
			{
				return m_shutdown || !m_queue.empty();
			});
		if (m_shutdown)
		{
			throw std::runtime_error("Event Queue shutdown");
		}
		

		T item = std::move(m_queue.front());
		m_queue.pop();
		return item;
	}

	template<typename T>
	inline bool EventQueue<T>::Empty() const
	{
		std::lock_guard lock(m_mutex);

		return m_queue.empty();
	}

	template<typename T>
	inline size_t EventQueue<T>::Size() const
	{
		std::lock_guard lock(m_mutex);

		return m_queue.size();
	}

	template<typename T>
	inline void EventQueue<T>::Shutdown()
	{
		{
			std::lock_guard lock(m_mutex);
			m_shutdown = true;
		}
		m_condition.notify_all();
	}

}