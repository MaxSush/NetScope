#pragma once
#include <event/NetworkEvent.h>
#include <model/FlowResult.h>
#include <model/ProcessResult.h>

#include <optional>

namespace netscope::processing
{
	struct ProcessingContext
	{
		explicit ProcessingContext(const NetworkEvent& event)
			: input(event)
		{
		}

		ProcessingContext(const ProcessingContext&) = delete;
		ProcessingContext& operator=(const ProcessingContext&) = delete;

		const NetworkEvent& input;

		std::optional<flow::FlowResult> flow;
		
		std::vector<flow::FlowResult> closedFlows;

		std::optional<process::ProcessResult> process;

		bool drop = false;
	};
}