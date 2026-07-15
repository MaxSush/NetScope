#include "FlowProcessor.h"

namespace netscope::processing
{


	FlowProcessor::FlowProcessor(process::ProcessManager& processManager, flow::FlowManager& flowManager, core::DomainEventBus<FlowUpdatedEvent>& eventBus)
		: m_processManager(processManager),
		m_flowManager(flowManager),
		m_eventBus(eventBus)
	{
	}

	void FlowProcessor::Process(const NetworkEvent& event)
	{
		auto process = m_processManager.FindProcess(event.pid);

		auto& flow = m_flowManager.Update(event);

		FlowUpdatedEvent updated{ process.value(), flow.ToView()};

		m_eventBus.Broadcast(updated);
	}

}