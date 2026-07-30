#pragma once

#include <chrono>

namespace netscope::config
{
	constexpr auto FLOW_IDLE_TIMEOUT = std::chrono::seconds(30);
	constexpr auto FLOW_ARCHIVE_TIMEOUT = std::chrono::seconds(60);

	constexpr auto LIFECYCLE_SCHEDULER_INTERVAL = std::chrono::seconds(5);
}