#include "ProcessManager.h"

#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

namespace netscope::process
{
	std::vector<ProcessInfo> ProcessManager::GetProcesses() const
	{
		std::vector<ProcessInfo> processes;

		for (const auto& entry : fs::directory_iterator("/proc"))
		{
			if (!entry.is_directory())
			{
				continue;
			}

			std::string name = entry.path().filename().string();
			if (!IsPidDirectory(name))
			{
				continue;
			}

			uint32_t pid = static_cast<uint32_t>(std::stoul(name));
			processes.push_back(ReadProcess(pid));
		}

		return processes;
	}

	std::optional<ProcessInfo> ProcessManager::FindProcess(uint32_t pid) const
	{
		std::string proc_path = "/proc/" + std::to_string(pid);
		if (!fs::exists(proc_path))
		{
			return std::nullopt;
		}

		return ReadProcess(pid);
	}

	bool ProcessManager::IsPidDirectory(const std::string& name) const
	{
		return !name.empty() && std::all_of(name.begin(), name.end(), ::isdigit);
	}

	ProcessInfo ProcessManager::ReadProcess(const uint32_t pid) const
	{
		ProcessInfo info{};
		info.pid = pid;

		std::string proc_path = "/proc/" + std::to_string(pid);
		// Read process name
		{
			std::ifstream file(proc_path + "/comm");
			if (file)
			{
				std::getline(file, info.name);
			}
		}
		
		// Read process start time
		{
			std::ifstream file(proc_path + "/stat");
			if (file)
			{
				std::string line;
				std::getline(file, line);
				auto pos = line.rfind(')');
				std::istringstream iss(line.substr(pos + 2));

				std::string temp;
				for (int i = 0;i < 19; ++i)
				{
					iss >> temp;
				}
				
				iss >> info.start_time;
			}
		}

		// Read executable path
		{
			std::error_code ec;
			auto exe_path = fs::read_symlink(proc_path + "/exe", ec);
			if (!ec)
			{
				info.exe_path = exe_path.string();
			}
		}
		return info;
	}

}

