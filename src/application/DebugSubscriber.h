#pragma once
#include <event/IEventSubscriber.h>
#include <activity/FlowUpdatedEvent.h>
#include <logger/Logger.h>

namespace netscope
{
	class DebugSubscriber : public core::IEventSubscriber<processing::FlowUpdatedEvent>
	{
	public:
		void HandleEvent(const processing::FlowUpdatedEvent& event) override
		{
			LOG_INFO("Flow Updated: PID {}", event.flowSnapshot.pid);
		}
	};

}