#pragma once

#include "FlowHash.h"
#include "model/Flow.h"
#include <time/TimeUtils.h>

#include <unordered_map>
#include <shared_mutex>

namespace netscope
{
	class FlowTable
	{
	public:
		FlowTable() = default;

		Flow* Find(const FlowKey& key);
		Flow& GetOrDefault(const FlowKey& key, const pid_t pid);

		void Remove(const FlowKey& key);
		void Clear();

		size_t Size() const;
		bool Empty() const;

		size_t RemoveIdleFlows(Duration timeout);

		std::vector<FlowView> GetSnapshot() const;

		template<typename Func>
		void ForEach(Func&& func) const;

	private:
		std::unordered_map<FlowKey, Flow> m_flows;

		mutable std::shared_mutex m_mutex;
	};

	template<typename Func>
	inline void FlowTable::ForEach(Func&& func) const
	{
		std::shared_lock lock(m_mutex);

		for (const auto& [key, flow] : m_flows)
		{
			func(key, flow);
		}
	}
}
