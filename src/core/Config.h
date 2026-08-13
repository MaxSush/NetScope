#pragma once

#include <chrono>

namespace netscope::config
{
	constexpr auto FLOW_IDLE_TIMEOUT = std::chrono::seconds(30);

	constexpr auto FLOW_EVICTION_TIMEOUT = std::chrono::seconds(60);
	constexpr auto PROCESS_EVICTION_TIMEOUT = std::chrono::seconds(60);

	constexpr auto FLOW_TCP_IDLE_HARD_TIMEOUT = std::chrono::minutes(10);

	constexpr auto LIFECYCLE_SCHEDULER_INTERVAL = std::chrono::seconds(5);
}