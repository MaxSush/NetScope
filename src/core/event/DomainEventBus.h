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

		void Publish(const T& event) const;
	private:
		std::vector<IEventSubscriber<T>*> m_subscribers;
	};

	template<typename T>
	inline void DomainEventBus<T>::Subscribe(IEventSubscriber<T>& subscriber)
	{
		if (std::find(m_subscribers.begin(), m_subscribers.end(), &subscriber) == m_subscribers.end())
		{
			m_subscribers.push_back(&subscriber);
		}
	}

	template<typename T>
	inline void DomainEventBus<T>::Unsubscribe(IEventSubscriber<T>& subscriber)
	{
		auto it = std::remove(m_subscribers.begin(), m_subscribers.end(), &subscriber);
		m_subscribers.erase(it, m_subscribers.end());
	}

	template<typename T>
	inline void DomainEventBus<T>::Publish(const T& event) const
	{
		for (auto* sub : m_subscribers)
		{
			sub->HandleEvent(event);
		}
	}
}