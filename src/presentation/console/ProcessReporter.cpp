#include "ProcessReporter.h"
#include <iostream>
#include <fmt/core.h>
#include <format/ByteFormatter.h>

namespace netscope
{
	ProcessReporter::ProcessReporter(ProcessAggregator& processAggregator, FlowManager& flowManager)
		: m_processAggregator(processAggregator),
		m_flowManager(flowManager)
	{
	}

	void ProcessReporter::Report()
	{
		std::cout << "\033[2J\033[1;1H";

		fmt::print(
			"{:<8} {:<20} {:>8} {:>8} {:>15} {:>15}\n",
			"PID",
			"PROCESS",
			"ACTIVE",
			"IDLE",
			"UPLOAD",
			"DOWNLOAD");
		std::cout << "------------------------------------------------------------------------------------------------------" << std::endl;

		auto flows = m_flowManager.GetTable().GetSnapshot();
		std::vector<ProcessView> processes = m_processAggregator.Aggregate(flows);

		for (const auto& process : processes)
		{
			fmt::print(
				"{:<8} {:<20} {:>8} {:>8} {:>15} {:>15}\n",
				process.pid,
				process.processName,
				process.activeConnections,
				process.idleConnections,
				ByteFormatter::Format(process.statistics.uploadBytes),
				ByteFormatter::Format(process.statistics.downloadBytes)
			);
		}

	}

}
