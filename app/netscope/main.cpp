#include <time/Timer.h>
#include <logger/Logger.h>

#include <thread>
#include <iostream>
#include <NetscopeApplication.h>

using namespace netscope;

int main()
{
	Logger::Initialize();
	Timer timer;

	LOG_INFO("NetScope started.");

	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	
	LOG_DEBUG("time elapsed seconds: {:.2f}", timer.ElapsedSeconds());
	LOG_DEBUG("time elapsed milliseconds: {:.2f}", timer.ElapsedMilliSeconds());

	fmt::print("Welcome to NetScope\n");

	// Test

	NetscopeApplication app;

	if (!app.Start())
	{
		return 1;
	}

	std::this_thread::sleep_for(std::chrono::seconds(120));

	app.Stop();

	return 0;
}