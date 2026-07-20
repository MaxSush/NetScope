#include "FlowTable.h"
#include <utility>
#include <mutex>

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

	std::pair<std::reference_wrapper<Flow>, bool> FlowTable::GetOrDefault(const FlowKey& key, const pid_t pid)
	{
		std::lock_guard lock(m_mutex);

		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			return std::make_pair(std::ref(it->second), false);
		}
		auto result = m_flows.emplace(key, Flow(key, pid));
		return std::make_pair(std::ref(result.first->second), true);
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
