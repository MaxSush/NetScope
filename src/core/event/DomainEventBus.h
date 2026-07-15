#pragma once

#include "IEventSubscriber.h"
#include <vector>
#include <algorithm>

namespace netscope::core
{
	template<typename T>
	class DomainEventBus
	{
	public:
		void Subscribe(IEventSubscriber<T>& subscriber);

		void Unsubscribe(IEventSubscriber<T>& subscriber);

		void Broadcast(const T& event);
	private:
		std::vector<IEventSubscriber<T>*> m_subscribers;
	};

	template<typename T>
	inline void DomainEventBus<T>::Subscribe(IEventSubscriber<T>& subscriber)
	{
		m_subscribers.push_back(&subscriber);
	}

	template<typename T>
	inline void DomainEventBus<T>::Unsubscribe(IEventSubscriber<T>& subscriber)
	{
		auto it = std::remove(m_subscribers.begin(), m_subscribers.end(), &subscriber);
		m_subscribers.erase(it, m_subscribers.end());
	}

	template<typename T>
	inline void DomainEventBus<T>::Broadcast(const T& event)
	{
		for (auto* sub : m_subscribers)
		{
			sub->HandleEvent(event);
		}
	}
}