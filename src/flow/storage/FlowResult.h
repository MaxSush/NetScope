#pragma once
#include <functional>
#include <model/Flow.h>

namespace netscope::flow
{
	enum class FlowChange
	{
		CREATED,
		UPDATED,
        IDLE,
        RESUMED,
        CLOSED,
        ARCHIVED
	};

	struct FlowResult
	{
		std::reference_wrapper<const Flow> flow;
		FlowChange change;

        const Flow& GetFlow() const
        {
            return flow.get();
        }

        FlowSnapshot GetSnapshot() const
        {
            return flow.get().GetSnapshot();
        }

        bool IsCreated() const
        {
            return change == FlowChange::CREATED;
        }

        bool IsUpdated() const
        {
            return change == FlowChange::UPDATED;
        }

        bool IsClosed() const
        {
            return change == FlowChange::CLOSED;
        }

	};
}