#pragma once
#include <pipeline/ProcessingContext.h>
#include <event/DomainEventBus.h>
#include <activity/FlowUpdatedEvent.h>
#include <activity/FlowCreatedEvent.h>
#include <activity/FlowClosedEvent.h>
#include <activity/ProcessClosedEvent.h>
#include <pipeline/ILifecycleProcessor.h>

namespace netscope::processing
{
	class NetworkCoordinator
	{
	public:
		NetworkCoordinator();

		void Coordinate(const ProcessingContext& context);
		void Coordinate(const LifecycleContext& context);

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
		core::DomainEventBus<FlowClosedEvent> m_flowClosedBus;
		core::DomainEventBus<ProcessClosedEvent> m_processClosedBus;
	};

	template<typename TSub>
	inline void NetworkCoordinator::Subscribe(TSub& subscriber)
	{
		SubscribeIfSupported<FlowCreatedEvent>(subscriber, m_flowCreatedBus);
		SubscribeIfSupported<FlowUpdatedEvent>(subscriber, m_flowUpdatedBus);
		SubscribeIfSupported<FlowClosedEvent>(subscriber, m_flowClosedBus);
		SubscribeIfSupported<ProcessClosedEvent>(subscriber, m_processClosedBus);

	}

	template<typename TSub>
	inline void NetworkCoordinator::Unsubscribe(TSub& subscriber)
	{
		UnsubscribeIfSupported<FlowCreatedEvent>(subscriber, m_flowCreatedBus);
		UnsubscribeIfSupported<FlowUpdatedEvent>(subscriber, m_flowUpdatedBus);
		UnsubscribeIfSupported<FlowClosedEvent>(subscriber, m_flowClosedBus);
		UnsubscribeIfSupported<ProcessClosedEvent>(subscriber, m_processClosedBus);

	}

}