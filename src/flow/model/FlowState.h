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

	enum class CloseReason
	{
		TCP_FIN,
		TCP_RESET,
		SOCKET_DESTROYED,
		PROCESS_EXITED,
		INTEFACE_DOWN,
		CAPTURE_STOPPED,
		NOT_CLOSED,
		UNKNOWN
	};

	inline constexpr const char* ToCloseReasonString(CloseReason reason)
	{
		switch (reason)
		{
		case CloseReason::TCP_FIN:
			return "TCP_FIN";
		case CloseReason::TCP_RESET:
			return "TCP_RESET";
		case CloseReason::SOCKET_DESTROYED:
			return "SOCKET_DESTROYED";
		case CloseReason::PROCESS_EXITED:
			return "PROCESS_EXITED";
		case CloseReason::INTEFACE_DOWN:
			return "INTEFACE_DOWN";
		case CloseReason::CAPTURE_STOPPED:
			return "CAPTURE_STOPPED";
		case CloseReason::NOT_CLOSED:
			return "NOT_CLOSED";
		default:
			return "UNKNOWN";
		}
	}
}