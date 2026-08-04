#pragma once


namespace netscope
{
	enum class NetworkEventType
	{
		PACKET,
		CONNECTIONCLOSED
	};

	enum class CloseReason
	{
		NONE,
		TCP_FIN,
		TCP_RESET,
		SOCKET_DESTROYED,
		INTERFACE_DOWN,
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
		case CloseReason::INTERFACE_DOWN:
			return "INTERFACE_DOWN";
		case CloseReason::CAPTURE_STOPPED:
			return "CAPTURE_STOPPED";
		case CloseReason::NOT_CLOSED:
			return "NOT_CLOSED";
		default:
			return "UNKNOWN";
		}
	}

}