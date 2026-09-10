#include "ConfigLoader.h"

#include <filesystem>
#include <logger/Logger.h>
#include <toml++/toml.h>

namespace fs = std::filesystem;

namespace netscope
{
	Config ConfigLoader::Load(const std::optional<std::string>& overridePath)
	{
		Config config;

		// SYSTEM-WIDE CONFIG
		const std::string systemPath = "/etc/netscope/config.toml";
		if (fs::exists(systemPath))
		{
			LOG_INFO("Loading system config: {}", systemPath);
			ApplyFile(config, systemPath);
		}

		// PER-USER CONFIG
		const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
		std::string userPath;
		if (xdgConfig)
		{
			userPath = std::string(xdgConfig) + "/netscope/config.toml";
		}
		else
		{
			const char* home = std::getenv("HOME");
			if (home)
			{
				userPath = std::string(home) + "/.config/netscope/config.toml";
			}
		}

		if (!userPath.empty() && fs::exists(userPath))
		{
			LOG_INFO("Loading user config: {}", userPath);
			ApplyFile(config, userPath);
		}

		// CLI OVERRIDE
		if (overridePath.has_value() && fs::exists(overridePath.value()))
		{
			LOG_INFO("Loading override config: {}", overridePath.value());
			ApplyFile(config, overridePath.value());
		}

		return config;
	}

	void ConfigLoader::ApplyFile(Config& config, const std::string& path)
	{
		try
		{
			auto tbl = toml::parse_file(path);

			// GENERAL
			if (auto v = tbl["general"]["database_path"].value<std::string>())
			{
				config.databasePath = *v;
			}
			if (auto v = tbl["general"]["bpf_object_path"].value<std::string>())
			{
				config.bpfObjectPath = *v;
			}
			if (auto v = tbl["general"]["log_level"].value<std::string>())
			{
				config.logLevel = *v;
			}

			// FLOW
			if (auto v = tbl["flow"]["idle_timeout_secs"].value<int64_t>())
			{
				config.flowIdleTimeout = Seconds(*v);
			}
			if (auto v = tbl["flow"]["tcp_idle_hard_timeout_secs"].value<int64_t>())
			{
				config.flowTcpIdleHardTimeout = Seconds(*v);
			}
			if (auto v = tbl["flow"]["udp_idle_close_timeout_secs"].value<int64_t>())
			{
				config.flowUdpIdleCloseTimeout = Seconds(*v);
			}
			if (auto v = tbl["flow"]["eviction_timeout_secs"].value<int64_t>())
			{
				config.flowEvictionTimeout = Seconds(*v);
			}

			// PROCESS
			if (auto v = tbl["process"]["eviction_timeout_secs"].value<int64_t>())
			{
				config.processEvictionTimeout = Seconds(*v);
			}
			if (auto arr = tbl["process"]["ignored_processes"].as_array())
			{
				config.ignoredProcesses.clear();
				for (const auto& elem : *arr)
				{
					if (auto str = elem.value<std::string>())
					{
						config.ignoredProcesses.insert(*str);
					}
				}
			}

			// PERSISTENCE
			if (auto v = tbl["persistence"]["batch_size"].value<int64_t>())
			{
				config.persistenceBatchSize = static_cast<size_t>(*v);
			}
			if (auto v = tbl["persistence"]["batch_delay_ms"].value<int64_t>())
			{
				config.persistenceBatchDelay = Milliseconds(*v);
			}

			// RETENTION
			if (auto v = tbl["retention"]["retention_hours"].value<int64_t>())
			{
				config.retentionHours = static_cast<uint32_t>(*v);
			}
			if (auto v = tbl["retention"]["hourly_retention_days"].value<int64_t>())
			{
				config.hourlyRetentionDays = static_cast<uint32_t>(*v);
			}
			if (auto v = tbl["retention"]["daily_retention_days"].value<int64_t>())
			{
				config.dailyRetentionDays = static_cast<uint32_t>(*v);
			}

			// LIFECYCLE
			if (auto v = tbl["lifecycle"]["interval_secs"].value<int64_t>())
			{
				config.lifecycleInterval = Seconds(*v);
			}

			// DISPLAY
			if (auto v = tbl["display"]["report_interval_secs"].value<int64_t>())
			{
				config.reportInterval = Seconds(*v);
			}
		}
		catch (toml::parse_error& err)
		{
			LOG_ERROR("Failed to parse config file '{}' : {}", path, err.what());
			// Continue without error [only log error]
		}
	}
}
