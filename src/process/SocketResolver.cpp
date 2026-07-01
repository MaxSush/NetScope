#include "SocketResolver.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace netscope
{
	std::vector<SocketInfo> SocketResolver::GetSockets(uint32_t pid) const
	{
		std::vector<SocketInfo> sockets;

		std::string path = "/proc/" + std::to_string(pid) + "/fd";
		std::error_code ec;

		fs::directory_iterator it(path, ec);
		if (ec)
		{
			return sockets;
		}

		for (const auto& entry : it)
		{
			auto target = fs::read_symlink(entry.path(), ec);
			if (ec)
			{
				continue;
			}

			std::string link = target.string();
			if (!IsSocket(link))
			{
				continue;
			}

			SocketInfo info{};
			info.pid = pid;
			info.inode = ExtractInode(link);

			sockets.push_back(info);
		}

		return sockets;
	}

	bool SocketResolver::IsSocket(const std::string& target) const
	{
		return target.find("socket:[") == 0;
	}

	uint64_t SocketResolver::ExtractInode(const std::string& target) const
	{
		auto start = target.find('[');
		auto end = target.find(']');

		std::string inode = target.substr(start + 1, end - start - 1);

		return std::stoull(inode);
	}

}

