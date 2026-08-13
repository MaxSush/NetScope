#include "FlowManager.h"

#include <context/LifecycleContext.h>
#include <stdexcept>

namespace netscope::flow
{
	void FlowManager::Process(processing::ProcessingContext& context)
	{
		const NetworkEvent& event = context.input;

		switch (context.input.type)
		{
		[[likely]] case NetworkEventType::PACKET:
		{
			Endpoint source{ event.sourceAddress, event.sourcePort };
			Endpoint destination{ event.destinationAddress, event.destinationPort };
			FlowKey key = FlowKey::Create(source, destination, event.protocol);

			context.flow = m_Table.GetOrDefault(key, event);
			break;
		}
		[[unlikely]]  case NetworkEventType::CONNECTION_CLOSED:
		{
			Endpoint source{ event.sourceAddress, event.sourcePort };
			Endpoint destination{ event.destinationAddress, event.destinationPort };
			FlowKey key = FlowKey::Create(source, destination, event.protocol);

			context.flow = m_Table.CloseFlows(key, context.input.close);
			break;
		}
		[[unlikely]] case NetworkEventType::PROCESS_EXITED:
		{
			context.closedFlows = m_Table.CloseFlowsByPid(context.input.pid, CloseReason::PROCESS_EXITED);
			break;
		}
		}
	}

	void FlowManager::Process(processing::LifecycleContext& context)
	{
		m_Table.UpdateFlowLifeCycle(context);
	}

	void FlowManager::CleanUpExpired(Steady_TimePoint now)
	{
		m_Table.CleanUpClosedFlows(now);
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
