#pragma once
#include <shared/NetworkEvent.h>
#include <optional>
#include <storage/FlowResult.h>
#include <ProcessResult.h>

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
		std::optional<process::ProcessResult> process;
	};
}