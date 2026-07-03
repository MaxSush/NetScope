#pragma once

#include "storage/FlowTable.h"

namespace netscope
{
	class FlowManager
	{
	public:
		FlowManager() = default;

		void ProcessPacket(const PacketEvent& event);
		const FlowTable& GetTable() const;
		void Clear();

	private:
		FlowTable m_Table;

	};
}