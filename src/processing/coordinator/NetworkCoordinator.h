#pragma once
#include<context/ProcessingContext.h>
#include <event/DomainEventBus.h>
#include <activity/FlowUpdatedEvent.h>
#include <activity/FlowCreatedEvent.h>

namespace netscope::processing
{
	class NetworkCoordinator
	{
	public:
		NetworkCoordinator();

		void Coordinate(const ProcessingContext& context);

		template<typename TSub>
		void Subscribe(TSub& subscriber);

		template<typename TSub>
		void Unsubscribe(TSub& subscriber);

	private:

		template<typename TEvent, typename TSub>
		void SubscribeIfSupported(TSub& subscriber, core::DomainEventBus<TEvent>& bus)
		{
			if constexpr (std::is_base_of_v<core::IEventSubscriber<TEvent>, TSub>)
			{
				bus.Subscribe(subscriber);
			}
		}

		template<typename TEvent, typename TSub>
		void UnsubscribeIfSupported(TSub& subscriber, core::DomainEventBus<TEvent>& bus)
		{
			if constexpr (std::is_base_of_v<core::IEventSubscriber<TEvent>, TSub>)
			{
				bus.Unsubscribe(subscriber);
			}
		}

		core::DomainEventBus<FlowUpdatedEvent> m_flowUpdatedBus;
		core::DomainEventBus<FlowCreatedEvent> m_flowCreatedBus;

	};

	template<typename TSub>
	inline void NetworkCoordinator::Subscribe(TSub& subscriber)
	{
		SubscribeIfSupported<FlowCreatedEvent>(subscriber, m_flowCreatedBus);
		SubscribeIfSupported<FlowUpdatedEvent>(subscriber, m_flowUpdatedBus);

	}

	template<typename TSub>
	inline void NetworkCoordinator::Unsubscribe(TSub& subscriber)
	{
		UnsubscribeIfSupported<FlowCreatedEvent>(subscriber, m_flowCreatedBus);
		UnsubscribeIfSupported<FlowUpdatedEvent>(subscriber, m_flowUpdatedBus);

	}

}