#pragma once

#include "database/DatabaseResult.h"
#include <utilities/Expected.h>

#include <vector>
#include <model/FlowSnapshot.h>

namespace netscope::storage
{
	class IFlowRepository
	{
	public:
		virtual DatabaseResult Upsert(const flow::FlowSnapshot& flowSnapshot, int64_t processId) = 0;
		//virtual Expected<std::vector<FlowView>, DatabaseResult> FindByProcess(uint64_t processId) = 0;
		//virtual DatabaseResult Close(uint64_t flowId) = 0;
	};
}