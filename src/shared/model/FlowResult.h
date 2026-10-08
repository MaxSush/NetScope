#pragma once

#include "FlowSnapshot.h"

namespace netscope::flow
{
	enum class FlowChange
	{
		CREATED,
		UPDATED,
        IDLE,
        RESUMED,
        CLOSED
	};

	struct FlowResult
	{
        FlowSnapshot snapshot;
		FlowChange change;

        FlowSnapshot GetSnapshot() const
        {
            return snapshot;
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