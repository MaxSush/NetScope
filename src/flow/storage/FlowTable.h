#pragma once

#include "FlowHash.h"
#include <time/TimeUtils.h>
#include "FlowResult.h"

#include <unordered_map>
#include <shared_mutex>

namespace netscope::processing
{
	struct LifecycleContext;
}

namespace netscope::flow
{
	class FlowTable
	{
	public:
		FlowTable() = default;

		Flow* Find(const FlowKey& key);
		FlowResult GetOrDefault(const FlowKey& key, const NetworkEvent& event);

		std::optional<FlowResult> CloseFlows(const FlowKey& key, FlowTerminationInfo info);
		std::vector<FlowResult> CloseFlowsByPid(const uint32_t pid, CloseReason reason);

		void Remove(const FlowKey& key);
		void Clear();

		size_t Size() const;
		bool Empty() const;

		void UpdateFlowLifeCycle(processing::LifecycleContext& context);
		void CleanUpClosedFlows(Steady_TimePoint now);

		std::vector<FlowSnapshot> GetSnapshot() const;

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
