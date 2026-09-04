#pragma once

#include <string>
#include <time/TimeUtils.h>
#include <unordered_set>
#include <optional>

namespace netscope
{
	struct Config
	{
	public:
		static void Initialize(Config config)
		{
			s_instance.emplace(std::move(config));
		}

		static const Config& Get()
		{
			return s_instance.value();
		}

	public:
		// GENERAL
		std::string databasePath = "netscope.db";
		std::string bpfObjectPath = "packet_monitor.bpf.o";
		std::string logLevel = "info";

		// FLOW LIFECYCLE
		Seconds flowIdleTimeout{ 30 };
		Seconds flowTcpIdleHardTimeout{ 600 };
		Seconds flowUdpIdleCloseTimeout{ 60 };
		Seconds flowEvictionTimeout{ 60 };

		// PROCESS
		Seconds processEvictionTimeout{ 60 };
		std::unordered_set<std::string> ignoredProcesses;

		// PERSISTENCE
		size_t persistenceBatchSize = 1000;
		Milliseconds persistenceBatchDelay{ 1000 };
		uint32_t retentionHours = 0;

		// LIFECYCLE SCHEDULER
		Seconds lifecycleInterval{ 5 };

		// DISPLAY
		Seconds reportInterval{ 1 };

	private:
		static std::optional<Config> s_instance;
	};

	inline std::optional<Config> Config::s_instance;
}