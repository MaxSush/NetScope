#include "FlowTable.h"
#include <utility>
#include <mutex>
#include <context/LifecycleContext.h>
#include <unordered_set>

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
			// Flow Already Closed
			if (it->second.GetState() == FlowState::CLOSED)
			{
				return { std::ref(it->second), FlowChange::UPDATED };
			}

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

	std::optional<FlowResult> FlowTable::CloseFlow(const FlowKey& key, FlowTerminationInfo info)
	{
		std::lock_guard lock(m_mutex);

		auto it = m_flows.find(key);
		if (it == m_flows.end())
		{
			return std::nullopt;
		}

		if (it->second.GetState() == FlowState::CLOSED)
		{
			return std::nullopt;
		}

		it->second.SetState(FlowState::CLOSED);
		it->second.SetCloseInfo(info);
		
		return FlowResult{ std::ref(it->second), FlowChange::CLOSED };
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

	void FlowTable::UpdateFlowLifeCycle(processing::LifecycleContext& context)
	{
		{
			std::lock_guard lock(m_mutex);

			std::unordered_set<uint32_t> exitedSet(context.exitedPids.begin(), context.exitedPids.end());

			for (auto& [key, flow] : m_flows)
			{
				if (flow.GetState() == FlowState::CLOSED)
				{
					continue;
				}

				// Process Exit detection
				if (!exitedSet.empty() && exitedSet.count(flow.GetPID()))
				{
					flow.SetState(FlowState::CLOSED);
					flow.SetCloseReason(CloseReason::PROCESS_EXITED);
					context.flowChanges.emplace_back(FlowResult{ flow, FlowChange::CLOSED });
					continue;
				}

				// Active -> Idle transition
				if (flow.GetState() == FlowState::ACTIVE && flow.IsIdle(context.now))
				{
					flow.SetState(FlowState::IDLE);
					context.flowChanges.emplace_back(FlowResult{ flow, FlowChange::IDLE });
					continue;
				}

				// IDLE Flows timeout
				if (flow.GetState() == FlowState::IDLE)
				{
					bool shouldClose = false;

					if (flow.GetKey().protocol == Protocol::UDP)
					{
						shouldClose = flow.CanEvict(context.now);
					}
					else if (flow.GetKey().protocol == Protocol::TCP)
					{
						shouldClose = flow.HasTcpIdleTimeout(context.now);
					}

					if (shouldClose)
					{
						flow.SetState(FlowState::CLOSED);
						flow.SetCloseReason(CloseReason::IDLE_TIMEOUT);
						context.flowChanges.emplace_back(FlowResult{ flow, FlowChange::CLOSED });
					}
				}

			}
		}
	}

	void FlowTable::CleanUpClosedFlows(Steady_TimePoint now)
	{
		std::lock_guard lock(m_mutex);

		for (auto it = m_flows.begin(); it != m_flows.end(); )
		{
			if (it->second.CanEvict(now))
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
