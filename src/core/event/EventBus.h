#pragma once

#include <algorithm>
#include <functional>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <cstdint>

#include "logger/Logger.h"


namespace netscope
{
	class EventBus
	{
	public:
		using SubscriptionID = uint64_t;

		template<typename Event>
		static SubscriptionID Subscribe(std::function<const Event&> callback);

		template<typename Event>
		static void Unsubscribe(SubscriptionID id);

		template<typename Event>
		static void Publish(const Event& event);

		template<typename Event>
		static inline size_t SubscriberCount();

		static void Clear();

	private:
		struct Subscriber
		{
			SubscriptionID id;
			std::function<void(const void*)> callback;
		};

		static std::unordered_map<std::type_index, std::vector<Subscriber>> m_subscribers;
		static std::mutex m_mutex;
		static std::atomic<SubscriptionID> count;
	};


	template<typename Event>
	inline EventBus::SubscriptionID EventBus::Subscribe(std::function<const Event&> callback)
	{
		std::lock_guard lock(m_mutex);

		SubscriptionID id = ++count;

		m_subscribers[typeid(Event)].push_back(
			{
				id,
				[callback](const void* event)
				{
					callback(*static_cast<const Event*>(event));
				}
			}
		);

		return id;
	}

	template<typename Event>
	inline void EventBus::Unsubscribe(SubscriptionID id)
	{
		std::lock_guard lock(m_mutex);

		auto it = m_subscribers.find(typeid(Event));
		if (it == m_subscribers.end())
		{
			return;
		}
		auto& subscribers = it->second;
		subscribers.erase(
			std::remove_if(subscribers.begin(), subscribers.end(), 
				[id](const EventBus::Subscriber& s)
				{
					return s.id == id;
				}
			),
			subscribers.end()
		);
	}

	template<typename Event>
	inline void EventBus::Publish(const Event& event)
	{
		std::vector<Subscriber> subscribers;
		
		{
			std::lock_guard lock(m_mutex);
			auto it = m_subscribers.find(typeid(Event));
			if (it == m_subscribers.end())
			{
				return;
			}
			subscribers = it->second;
		}

		for (auto& s : subscribers)
		{
			try
			{
				s.callback(&event);
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Event callback exception: {}", e.what());
			}
			catch (...)
			{
				LOG_ERROR("Unknown event callback exception.");
			}
		}
	}

	template<typename Event>
	inline size_t EventBus::SubscriberCount()
	{
		std::lock_guard lock(m_mutex);

		auto it =
			m_subscribers.find(typeid(Event));

		if (it == m_subscribers.end())
			return 0;

		return it->second.size();
	}

}
