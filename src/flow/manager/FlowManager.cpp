#include "FlowManager.h"

#include <context/LifecycleContext.h>
#include <stdexcept>

namespace netscope::flow
{
	void FlowManager::Process(processing::ProcessingContext& context)
	{
		const NetworkEvent& event = context.input;

		Endpoint source{
			event.sourceAddress,
			event.sourcePort
		};

		Endpoint destination{
			event.destinationAddress,
			event.destinationPort
		};

		FlowKey key = FlowKey::Create(source, destination, event.protocol);
		switch (context.input.type)
		{
		[[likely]] case NetworkEventType::PACKET:
			context.flow = m_Table.GetOrDefault(key, event);
			break;
		[[unlikely]]  case NetworkEventType::CONNECTIONCLOSED:
			context.flow = m_Table.CloseFlow(key, context.input.closeReason.value_or(CloseReason::UNKNOWN));
			break;
		default:
			std::runtime_error("Unsupported NetworkEventType");
		}
	}

	void FlowManager::Process(processing::LifecycleContext& context)
	{
		m_Table.UpdateFlowLifeCycle(context);
	}

	void FlowManager::CleanUpArchived()
	{
		m_Table.CleanUpArchivedFlows();
	}

	std::vector<FlowSnapshot> FlowManager::GetSnapshot() const
	{
		return m_Table.GetSnapshot();
	}

	void FlowManager::Clear()
	{
		m_Table.Clear();
	}
}
