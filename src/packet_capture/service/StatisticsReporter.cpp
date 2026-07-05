#include "StatisticsReporter.h"
#include <iostream>

namespace netscope
{

	StatisticsReporter::StatisticsReporter(StatisticsAggregator& aggregator, ProcessCache& m_processCache)
		: 
		m_aggregator(aggregator),
		m_processCache(m_processCache)
	{
	}

	void StatisticsReporter::Report()
	{
		auto snapshot = m_aggregator.GetSnapshot();

		for (const auto& [pid, stats] : snapshot)
		{
			auto process = m_processCache.FindProcess(pid);
			std::string name = (process.has_value()) ? process->name : "[Terminated]";
			
			std::cout
				<< pid
				<< "\t"
				<< name
				<< "\t"
				<< process->start_time
				<< "\t";
		}

		m_processCache.CleanUp(std::chrono::seconds(60));
	}

}