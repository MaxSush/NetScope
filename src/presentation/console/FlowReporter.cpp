#include "FlowReporter.h"
#include <iostream>
#include <chrono>

namespace netscope
{

	FlowReporter::FlowReporter(FlowManager& flowManager)
		: m_flowManager(flowManager)
	{
	}

	void FlowReporter::Report()
	{
		std::cout << "\033[2J\033[1;1H";
		const FlowTable& flowTable = m_flowManager.GetTable();

		std::vector<FlowView> flowViews = flowTable.GetSnapshot();

		for (const auto& view : flowViews)
		{
			std::cout << view.pid
				<< "\t" << view.key.source.ToString()
				<< " <==> " << view.key.destination.ToString()
				<< "\t" << ToProtocolString(view.key.protocol)
				<< "\tUP : " << view.statistics.uploadBytes << " " << view.statistics.uploadPackets
				<< "\tDOWN : " << view.statistics.downloadBytes << " " << view.statistics.downloadPackets
				<< "\t Duration: " << std::chrono::duration_cast<std::chrono::duration<double>>(view.lastSeen - view.firstSeen).count();

			std::cout << std::endl;
		}

	}

}
