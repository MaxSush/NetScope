#pragma once

namespace netscope::flow
{
	enum class FlowState
	{
		ACTIVE,
		IDLE,
		CLOSED,
		ARCHIVED
	};

	inline constexpr const char* ToFlowStateString(FlowState state)
	{
		switch (state)
		{
		case FlowState::ACTIVE:
			return "ACTIVE";
		case FlowState::IDLE:
			return "IDLE";
		case FlowState::CLOSED:
			return "CLOSED";
		case FlowState::ARCHIVED:
			return "ARCHIVED";
		default:
			return "UNKNOWN";
		}
	}

}