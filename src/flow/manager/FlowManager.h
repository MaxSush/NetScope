#pragma once

#include "storage/FlowTable.h"

namespace netscope::flow
{
	class FlowManager
	{
	public:
		FlowManager() = default;

		Flow& Update(const NetworkEvent& event);

		const FlowTable& GetTable() const;

		size_t RemoveIdleFlows(Duration timeout);

		void Clear();

	private:
		FlowTable m_Table;
	};
}