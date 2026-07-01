#pragma once

#include<vector>
#include <string>

#include "SocketInfo.h"

namespace netscope
{
	class SocketResolver
	{
	public:
		std::vector<SocketInfo> GetSockets(uint32_t pid) const;

	private:
		bool IsSocket(const std::string& target) const;
		uint64_t ExtractInode(const std::string& target) const;

	};
}