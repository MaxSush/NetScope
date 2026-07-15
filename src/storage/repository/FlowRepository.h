#pragma once
#include <database/IDatabase.h>
#include "IFlowRepository.h"

namespace netscope::storage
{
	class FlowRepository : IFlowRepository
	{
	public:
		explicit FlowRepository(IDatabase& database);

		DatabaseResult Upsert(const Flow& flow) override;
		//Expected<std::vector<FlowView>, DatabaseResult> FindByProcess(uint64_t processId) override;
		//DatabaseResult Close(uint64_t flowId) override;

	private:
		IDatabase& m_db;
	};
}