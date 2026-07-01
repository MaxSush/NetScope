
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace netscope
{
	void Logger::Initialize()
	{
		auto logger = spdlog::stderr_color_mt("netscope");
		logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");

#ifndef NDEBUG
		logger->set_level(spdlog::level::trace);
#else
		logger->set_level(spdlog::level::info);
#endif // !NDEBUG

		spdlog::set_default_logger(logger);

	}
}
