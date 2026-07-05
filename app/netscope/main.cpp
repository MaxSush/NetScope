#include <timer/Timer.h>
#include <logger/Logger.h>
#include <manager/FlowManager.h>

#include <thread>
#include <iostream>
#include <service/PacketCaptureService.h>

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

	PacketCaptureService ps;

	if (ps.Start())
	{
		std::this_thread::sleep_for(std::chrono::seconds(60));
	}


	ps.Stop();

	return 0;
}