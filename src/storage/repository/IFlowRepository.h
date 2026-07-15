#pragma once

#include "database/DatabaseResult.h"
#include <utilities/Expected.h>

#include <vector>
#include <model/Flow.h>

namespace netscope::storage
{
	class IFlowRepository
	{
	public:
		virtual DatabaseResult Upsert(const flow::Flow& flow) = 0;
		//virtual Expected<std::vector<FlowView>, DatabaseResult> FindByProcess(uint64_t processId) = 0;
		//virtual DatabaseResult Close(uint64_t flowId) = 0;
	};
}