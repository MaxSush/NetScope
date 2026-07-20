#include "FlowManager.h"

namespace netscope::flow
{
	size_t FlowManager::RemoveIdleFlows(Duration timeout)
	{
		return m_Table.RemoveIdleFlows(timeout);
	}

	void FlowManager::Process(processing::ProcessingContext& context)
	{
		context.flow = Update(context.input);
	}

	FlowResult FlowManager::Update(const NetworkEvent& event)
	{
		Endpoint source{
			event.sourceAddress,
			event.sourcePort
		};

		Endpoint destination{
			event.destinationAddress,
			event.destinationPort
		};

		FlowKey key = FlowKey::Create(source, destination, event.protocol);

		auto [flow, created] = m_Table.GetOrDefault(key, event.pid);
		flow.get().Update(event);

		return { flow, (created) ? FlowChange::Created : FlowChange::Updated };
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
