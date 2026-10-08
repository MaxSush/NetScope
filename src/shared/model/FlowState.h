#pragma once

namespace netscope::flow
{
	enum class FlowState
	{
		ACTIVE,
		IDLE,
		CLOSED
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
		default:
			return "UNKNOWN";
		}
	}

}