#include <time/Timer.h>
#include <logger/Logger.h>

#include <thread>
#include <iostream>
#include <NetscopeApplication.h>
#include <config/ConfigLoader.h>

#include <csignal>

using namespace netscope;

std::atomic<bool> g_running(true);

void SignalHandler(int)
{
	g_running = false;
}

int main(int argc, char* argv[])
{
	Logger::Initialize();

	std::optional<std::string> configPath;
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg == "--config" && i + 1 < argc)
		{
			configPath = argv[++i];
		}
	}

	Config::Initialize(ConfigLoader::Load(configPath));

	const auto& config = Config::Get();

	LOG_INFO("Netscope started.");
	LOG_INFO("Database: {}", config.databasePath);
	LOG_INFO("BPF Object: {}", config.bpfObjectPath);
	LOG_INFO("Ignored processes: {}", config.ignoredProcesses.size());

	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);

	NetscopeApplication app;

	if (!app.Start())
	{
		return 1;
	}

	while (g_running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	LOG_INFO("Shutdown signal received. Stopping NetScope...");

	app.Stop();

	return 0;
}