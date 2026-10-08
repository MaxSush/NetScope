#pragma once

#include <cstdint>

namespace netscope
{
	enum class CloseReason : uint8_t
	{
		NOT_CLOSED = 0,
		TCP_FIN,
		TCP_RESET,
		CONNECTION_REFUSED,
		TCP_TIMEOUT,

		PROCESS_EXITED,
		IDLE_TIMEOUT,
		KERNEL_ERROR,

		CAPTURE_STOPPED,
		UNKNOWN
	};

	inline constexpr const char* ToCloseReasonString(CloseReason reason)
	{
		switch (reason)
		{
		case CloseReason::NOT_CLOSED:
			return "NOT_CLOSED";
		case CloseReason::TCP_FIN:
			return "TCP_FIN";
		case CloseReason::TCP_RESET:
			return "TCP_RESET";
		case CloseReason::CONNECTION_REFUSED:
			return "CONNECTION_REFUSED";
		case CloseReason::TCP_TIMEOUT:
			return "TCP_TIMEOUT";
		case CloseReason::PROCESS_EXITED:
			return "PROCESS_EXITED";
		case CloseReason::IDLE_TIMEOUT:
			return "IDLE_TIMEOUT";
		case CloseReason::KERNEL_ERROR:
			return "KERNEL_ERROR";
		case CloseReason::CAPTURE_STOPPED:
			return "CAPTURE_STOPPED";
		case CloseReason::UNKNOWN:
			return "UNKNOWN";
		default:
			return "UNKNOWN";
		}
	}
}