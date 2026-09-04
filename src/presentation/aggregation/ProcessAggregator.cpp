#include "ProcessAggregator.h"
#include <config/Config.h>
#include <algorithm>

namespace netscope
{
	ProcessAggregator::ProcessAggregator(process::ProcessManager& processManager)
		: m_processManager(processManager)
	{
	}

	std::vector<flow::ProcessView> ProcessAggregator::Aggregate(const std::vector<flow::FlowSnapshot>& flows) const
	{
		std::unordered_map<pid_t, flow::ProcessView> processes;

		for (const auto& flow : flows)
		{
			auto [it, inserted] = processes.try_emplace(flow.pid);

			flow::ProcessView& process = it->second;
			
			if (inserted)
			{
				process.pid = flow.pid;
				if (auto info = m_processManager.Resolve(flow.pid))
				{
					process.processName = info->process.get().name;
				}
				else
				{
					process.processName = "[Terminated]";
				}
			}
			
			process.statistics += flow.statistics;

			if (flow.state == flow::FlowState::CLOSED)
			{
				++process.closeConnections;
			}
			else if (flow.IsIdle(Config::Get().flowIdleTimeout))
			{
				++process.idleConnections;
			}
			else
			{
				++process.activeConnections;
			}
		}

		std::vector<flow::ProcessView> result;
		result.reserve(processes.size());
		for (auto& [pid, process] : processes)
		{
			result.emplace_back(std::move(process));
		}

		std::sort(result.begin(), result.end(), [](const flow::ProcessView& a, const flow::ProcessView& b) {
			return a.pid < b.pid;
			});

		return result;
	}
}