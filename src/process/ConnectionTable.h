#pragma once

#include <unordered_map>
#include <optional>

#include "ConnectionInfo.h"
#include "ProcessManager.h"
#include "SocketResolver.h"

namespace netscope
{
	class ConnectionTable
	{
	public:
		ConnectionTable();

		void Build();
		std::optional<ConnectionInfo> Find(uint64_t inode) const;
		void DisplayAll() const;

	private:
		std::unordered_map<uint64_t, ConnectionInfo> m_table;

		ProcessManager pm;
		SocketResolver sr;

	};
}

