#include <timer/Timer.h>
#include <logger/Logger.h>
#include <manager/FlowManager.h>

#include <thread>
#include <iostream>

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

	//PacketEvent p1
	//{
	//	IPAddress::IPv4(192168152),
	//	22,
	//	IPAddress::IPv4(127001),
	//	63,
	//	Protocol::TCP,
	//	Direction::OUTGOING,
	//	64,
	//	1234,
	//	1234
	//};
	//PacketEvent p2
	//{
	//	IPAddress::IPv4(127001),
	//	63,
	//	IPAddress::IPv4(192168152),
	//	22,
	//	Protocol::TCP,
	//	Direction::INCOMMING,
	//	64,
	//	1234,
	//	1234
	//};

	//PacketEvent p3
	//{
	//	IPAddress::IPv4(127001),
	//	66,
	//	IPAddress::IPv4(192168152),
	//	22,
	//	Protocol::TCP,
	//	Direction::INCOMMING,
	//	64,
	//	1234,
	//	1234
	//};
	//PacketEvent p4
	//{
	//	IPAddress::IPv4(192168152),
	//	22,
	//	IPAddress::IPv4(127001),
	//	63,
	//	Protocol::UDP,
	//	Direction::OUTGOING,
	//	64,
	//	1234,
	//	1234
	//};

	//FlowManager fm;
	//	
	//fm.ProcessPacket(p1);
	//fm.ProcessPacket(p2);
	//fm.ProcessPacket(p3);
	//fm.ProcessPacket(p4);

	//fm.GetTable().ForEach(
	//	[](const FlowKey& key,
	//		const Flow& flow)
	//	{
	//		std::cout
	//			<< key.source.ToString()
	//			<< " -> "
	//			<< key.destination.ToString()
	//			<< '\n';

	//		std::cout
	//			<< "Upload Bytes: "
	//			<< flow.GetStatistics().uploadBytes
	//			<< '\n';

	//		std::cout
	//			<< "Download Bytes: "
	//			<< flow.GetStatistics().downloadBytes
	//			<< '\n';

	//		std::cout
	//			<< "Upload Packets: "
	//			<< flow.GetStatistics().uploadPackets
	//			<< '\n';

	//		std::cout
	//			<< "Download Packets: "
	//			<< flow.GetStatistics().downloadPackets
	//			<< '\n';
	//	}
	//);
	// Test 1

	

	return 0;
}