#pragma once
#include <database/IDatabase.h>
#include "IFlowRepository.h"

namespace netscope::storage
{
	class FlowRepository : IFlowRepository
	{
	public:
		explicit FlowRepository(IDatabase& database);

		// TODO: remove default value of nullptr
		DatabaseResult Upsert(const flow::FlowSnapshot& flowSnapshot, int64_t processId, const std::optional<std::string>& hostname = std::nullopt) override;
		//Expected<std::vector<FlowView>, DatabaseResult> FindByProcess(uint64_t processId) override;
		//DatabaseResult Close(uint64_t flowId) override;

	private:
		IDatabase& m_db;
	};
}