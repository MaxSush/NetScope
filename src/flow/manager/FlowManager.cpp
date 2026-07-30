#include "FlowManager.h"
#include "FlowManager.h"

#include <context/LifecycleContext.h>

namespace netscope::flow
{
	void FlowManager::Process(processing::ProcessingContext& context)
	{
		context.flow = Update(context.input);
	}

	void FlowManager::Process(processing::LifecycleContext& context)
	{
		m_Table.UpdateFlows(context);
	}

	void FlowManager::CleanUpArchived()
	{
		m_Table.CleanUpArchivedFlows();
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

		return m_Table.GetOrDefault(key, event);
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
