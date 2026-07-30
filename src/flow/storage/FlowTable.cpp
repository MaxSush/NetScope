#include "FlowTable.h"
#include "FlowTable.h"
#include "FlowTable.h"
#include <utility>
#include <mutex>
#include <context/LifecycleContext.h>

namespace netscope::flow
{
	Flow* FlowTable::Find(const FlowKey& key)
	{
		std::shared_lock lock(m_mutex);

		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	FlowResult FlowTable::GetOrDefault(const FlowKey& key, const NetworkEvent& event)
	{
		std::lock_guard lock(m_mutex);

		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			FlowChange change = FlowChange::UPDATED;
			if(it->second.GetState() == FlowState::IDLE)
			{
				it->second.SetState(FlowState::ACTIVE);
				change = FlowChange::RESUMED;
			}
			it->second.Update(event);
			return {std::ref(it->second), change };
		}
		auto result = m_flows.emplace(key, Flow(key, event.pid));
		result.first->second.Update(event);
		return { std::ref(result.first->second), FlowChange::CREATED};
	}

	void FlowTable::Remove(const FlowKey& key)
	{
		std::lock_guard lock(m_mutex);
		m_flows.erase(key);
	}

	void FlowTable::Clear()
	{
		std::lock_guard lock(m_mutex);

		m_flows.clear();
	}

	size_t FlowTable::Size() const
	{
		std::shared_lock lock(m_mutex);
		return m_flows.size();
	}

	bool FlowTable::Empty() const
	{
		std::shared_lock lock(m_mutex);
		return m_flows.empty();
	}

	void FlowTable::UpdateFlows(processing::LifecycleContext& context)
	{
		{
			std::lock_guard lock(m_mutex);
			for (auto& [key, flow] : m_flows)
			{
				if (flow.GetState() == FlowState::ACTIVE && flow.IsIdle(context.now))
				{
					flow.SetState(FlowState::IDLE);
					context.flowChanges.emplace_back(FlowResult{ flow, FlowChange::IDLE });
				}
				else if (flow.GetState() == FlowState::CLOSED && flow.CanArchive(context.now))
				{
					flow.SetState(FlowState::ARCHIVED);
					context.flowChanges.emplace_back(FlowResult{ flow, FlowChange::ARCHIVED });
				}
			}
		}
	}

	void FlowTable::CleanUpArchivedFlows()
	{
		std::lock_guard lock(m_mutex);

		for (auto it = m_flows.begin(); it != m_flows.end(); )
		{
			if (it->second.GetState() == FlowState::ARCHIVED)
			{
				it = m_flows.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	std::vector<FlowSnapshot> FlowTable::GetSnapshot() const
	{
		std::vector<FlowSnapshot> snapshot;
		{
			std::shared_lock lock(m_mutex);
			snapshot.reserve(m_flows.size());
			for (const auto& [key, flow] : m_flows)
			{
				snapshot.emplace_back(flow.GetSnapshot());
			}
		}
		return snapshot;
	}

}
