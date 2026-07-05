#include "FlowReporter.h"
#include <iostream>

namespace netscope
{

	FlowReporter::FlowReporter(FlowManager& flowManager)
		: m_flowManager(flowManager)
	{
	}

	void FlowReporter::Report()
	{
		FlowTable flowTable = m_flowManager.GetTable();
		flowTable.ForEach([](const FlowKey& key, const Flow& flow) {
			auto& stat = flow.GetStatistics();
			std::cout << key.source.ToString()
				<< " ==> " << key.destination.ToString()
				<< "\t" << ToProtocolString(key.protocol)
				<< "\tUP : " << stat.uploadBytes << " " << stat.uploadPackets
				<< "\tDOWN : " << stat.downloadBytes << " " << stat.downloadPackets;
			std::cout << std::endl;
			}
		);

	}

}
