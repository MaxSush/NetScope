#include <time/Timer.h>
#include <logger/Logger.h>

#include <thread>
#include <iostream>
#include <NetscopeApplication.h>
#include <config/ConfigLoader.h>


using namespace netscope;

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

	NetscopeApplication app;

	if (!app.Start())
	{
		return 1;
	}

	std::this_thread::sleep_for(std::chrono::seconds(200));

	app.Stop();

	return 0;
}