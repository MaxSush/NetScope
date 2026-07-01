#include <core/logger/Logger.h>
#include <core/timer/Timer.h>
#include <core/threading/Thread.h>
#include <core/event/EventQueue.h>
#include <process/ProcessManager.h>
#include <iostream>
#include <packet_capture/loader/BpfLoader.h>
#include <service/PacketCaptureService.h>
#include <process/ConnectionTable.h>

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


	// Test 1

	PacketCaptureService service;
	if (!service.Start())
	{
		return 1;
	}

	std::this_thread::sleep_for(std::chrono::seconds(30));

	service.Stop();

	return 0;
}