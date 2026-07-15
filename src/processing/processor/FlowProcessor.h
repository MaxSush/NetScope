#pragma once
#include "dispatcher/INetworkEventProcessor.h"
#include <activity/FlowUpdatedEvent.h>

#include <ProcessManager.h>
#include <manager/FlowManager.h>
#include <event/DomainEventBus.h>

namespace netscope::processing
{
	class FlowProcessor : public INetworkEventProcessor
	{
	public:
		FlowProcessor(process::ProcessManager& processManager, flow::FlowManager& flowManager, core::DomainEventBus<FlowUpdatedEvent>& eventBus);

		void Process(const NetworkEvent& event) override;

	private:
		process::ProcessManager& m_processManager;
		flow::FlowManager& m_flowManager;
		core::DomainEventBus<FlowUpdatedEvent>& m_eventBus;
	};
}