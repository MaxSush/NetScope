#include "FlowTable.h"

namespace netscope
{
	Flow* netscope::FlowTable::Find(const FlowKey& key)
	{
		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	Flow& FlowTable::GetOrDefault(const FlowKey& key)
	{
		auto it = m_flows.find(key);
		if (it != m_flows.end())
		{
			return it->second;
		}
		auto result = m_flows.emplace(key, Flow(key));
		return result.first->second;
	}

	void FlowTable::Remove(const FlowKey& key)
	{
		m_flows.erase(key);
	}

	void FlowTable::Clear()
	{
		m_flows.clear();
	}

	size_t FlowTable::Size() const
	{
		return m_flows.size();
	}

	bool FlowTable::Empty() const
	{
		return m_flows.empty();
	}

}
