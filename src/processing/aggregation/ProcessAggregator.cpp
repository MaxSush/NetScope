#include "ProcessAggregator.h"
#include <Config.h>
#include <algorithm>

namespace netscope
{
	ProcessAggregator::ProcessAggregator(ProcessCache& cache)
		: m_processCache(cache)
	{
	}

	std::vector<ProcessView> ProcessAggregator::Aggregate(const std::vector<FlowView>& flows) const
	{
		std::unordered_map<pid_t, ProcessView> processes;

		for (const auto& flow : flows)
		{
			auto [it, inserted] = processes.try_emplace(flow.pid);

			ProcessView& process = it->second;
			
			if (inserted)
			{

				process.pid = flow.pid;
				if (auto info = m_processCache.FindProcess(flow.pid))
				{
					process.processName = info->name;
				}
				else
				{
					process.processName = "[Terminated]";
				}
			}
			process.statistics += flow.statistics;
			if (flow.IsIdle(FLOW_IDLE_TIMEOUT))
			{
				++process.idleConnections;
			}
			else
			{
				++process.activeConnections;
			}
		}

		std::vector<ProcessView> result;
		result.reserve(processes.size());
		for (auto& [pid, process] : processes)
		{
			result.emplace_back(std::move(process));
		}

		std::sort(result.begin(), result.end(), [](const ProcessView& a, const ProcessView& b) {
			return (a.statistics.downloadBytes + a.statistics.uploadBytes) < (b.statistics.downloadBytes + b.statistics.uploadBytes);
			});

		return result;
	}
}

/*
for every FlowView
↓
Find ProcessView
↓
If missing
	create one
↓
statistics += flow.statistics
↓
connection count++
↓
if flow.IsIdle()
	idle++
else
	active++

Finally
unordered_map
↓
vector<ProcessView>
↓
return
*/