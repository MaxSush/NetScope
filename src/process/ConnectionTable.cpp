#include "ConnectionTable.h"
#include <iostream>

namespace netscope::process
{
	ConnectionTable::ConnectionTable()
	{
		pm = ProcessManager();
		sr = SocketResolver();
	}

	void ConnectionTable::Build()
	{
		m_table.clear();
		auto processes = pm.GetProcesses();
		for (const auto& process : processes)
		{
			auto sockets = sr.GetSockets(process.pid);
			for (const auto& socket : sockets)
			{
				ConnectionInfo info{};
				info.inode = socket.inode;
				info.process = process;

				m_table[socket.inode] = info;
			}
		}
	}

	std::optional<ConnectionInfo> ConnectionTable::Find(uint64_t inode) const
	{
		auto it = m_table.find(inode);
		if (it == m_table.end())
		{
			return std::nullopt;
		}
		return it->second;
	}

	void ConnectionTable::DisplayAll() const
	{
		for (const auto& [inode, info] : m_table)
		{
			std::cout << info.process.pid << "\t" << info.process.name << "\t" << info.process.start_time << "\t" << inode << "\n";
		}
	}

}

