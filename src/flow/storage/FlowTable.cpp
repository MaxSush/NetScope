#include "FlowTable.h"

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

	Flow& FlowTable::GetOrDefault(const FlowKey& key, const pid_t pid)
	{
		std::lock_guard lock(m_mutex);

		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			return it->second;
		}
		auto result = m_flows.emplace(key, Flow(key, pid));
		return result.first->second;
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

	size_t FlowTable::RemoveIdleFlows(Duration timeout)
	{
		size_t removed = 0;
		{
			std::lock_guard lock(m_mutex);

			for (auto it = m_flows.begin(); it != m_flows.end(); )
			{
				if (it->second.IsIdle(timeout))
				{
					it = m_flows.erase(it);
					++removed;
				}
				else
				{
					++it;
				}
			}
		}
		return removed;
	}

	std::vector<FlowView> FlowTable::GetSnapshot() const
	{
		std::vector<FlowView> snapshot;
		{
			std::shared_lock lock(m_mutex);
			snapshot.reserve(m_flows.size());
			for (const auto& [key, flow] : m_flows)
			{
				snapshot.emplace_back(flow.ToView());
			}
		}
		return snapshot;
	}

}
