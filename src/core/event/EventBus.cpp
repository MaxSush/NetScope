#include "EventBus.h"

namespace netscope
{

	std::unordered_map<std::type_index, std::vector<EventBus::Subscriber>> EventBus::m_subscribers;
	std::mutex EventBus::m_mutex;
	std::atomic<EventBus::SubscriptionID> count = 0;
	
	void EventBus::Clear()
	{
		std::lock_guard lock(m_mutex);
		m_subscribers.clear();
	}
}
