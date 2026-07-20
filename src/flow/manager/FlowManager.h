#pragma once

#include "storage/FlowTable.h"
#include <pipeline/IContextProcessor.h>

namespace netscope::flow
{
	class FlowManager : public processing::IContextProcessor
	{
	public:
		FlowManager() = default;

		void Process(processing::ProcessingContext& context) override;

		FlowResult Update(const NetworkEvent& event);

		std::vector<FlowSnapshot> GetSnapshot() const;

		size_t RemoveIdleFlows(Duration timeout);

		void Clear();

	private:
		FlowTable m_Table;
	};
}