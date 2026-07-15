#include "FlowManager.h"

namespace netscope::flow
{
	size_t FlowManager::RemoveIdleFlows(Duration timeout)
	{
		return m_Table.RemoveIdleFlows(timeout);
	}

	Flow& FlowManager::Update(const NetworkEvent& event)
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

		Flow& flow = m_Table.GetOrDefault(key, event.pid);

		flow.Update(event);

		return flow;
	}

	const FlowTable& FlowManager::GetTable() const
	{
		return m_Table;
	}

	void FlowManager::Clear()
	{
		m_Table.Clear();
	}

}
