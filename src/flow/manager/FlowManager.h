#pragma once

#include "storage/FlowTable.h"
#include <dispatcher/INetworkEventProcessor.h>

namespace netscope
{
	class FlowManager : public INetworkEventProcessor
	{
	public:
		FlowManager() = default;

		void Process(const NetworkEvent& event) override;
		const FlowTable& GetTable() const;
		void Clear();

	private:
		FlowTable m_Table;
	};
}