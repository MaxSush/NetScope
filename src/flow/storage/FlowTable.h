#pragma once

#include "FlowHash.h"
#include "model/Flow.h"

#include <unordered_map>

namespace netscope
{
	class FlowTable
	{
	public:
		FlowTable() = default;

		Flow* Find(const FlowKey& key);
		Flow& GetOrDefault(const FlowKey& key);

		void Remove(const FlowKey& key);
		void Clear();

		size_t Size() const;
		bool Empty() const;

		template<typename Func>
		void ForEach(Func&& func) const
		{
			for (const auto& [key, flow] : m_flows)
			{
				func(key, flow);
			}
		}

	private:
		std::unordered_map<FlowKey, Flow> m_flows;

	};
}
