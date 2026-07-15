#include "FlowManager.h"

namespace netscope
{
	void FlowManager::Process(const NetworkEvent& event)
	{
		// TODO: PacketEvent
		Endpoint source {
			event.sourceAddress,
			event.sourcePort
		};

		Endpoint destination {
			event.destinationAddress,
			event.destinationPort
		};

		FlowKey key = FlowKey::Create(source, destination, event.protocol);

		Flow& flow = m_Table.GetOrDefault(key, event.pid);

		flow.Update(event);
	}

	size_t FlowManager::RemoveIdleFlows(Duration timeout)
	{
		return m_Table.RemoveIdleFlows(timeout);
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
