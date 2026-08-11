#include "ProcessManager.h"
#include "ProcessManager.h"
#include "ProcessManager.h"
#include "ProcessManager.h"
#include "ProcessManager.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <utility>

namespace fs = std::filesystem;

namespace netscope::process
{
	void ProcessManager::Process(processing::ProcessingContext& context)
	{
		if (context.input.type != NetworkEventType::PACKET)
		{
			return;
		}
		context.process = Resolve(context.input.pid).value();
	}

	void ProcessManager::Process(processing::LifecycleContext& context)
	{
		// TODO: Process Exited logic
	}

	void ProcessManager::CleanUpExpired(Steady_TimePoint now)
	{
	}

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
			auto proc = ReadProcess(pid);
			if (proc)
			{
				processes.push_back(ReadProcess(pid).value());
			}
		}

		return processes;
	}

	std::optional<ProcessResult> ProcessManager::Resolve(uint32_t pid)
	{
		auto now = std::chrono::steady_clock::now();
		std::lock_guard lock(m_mutex);

		auto it = m_cache.find(pid);
		if (it != m_cache.end())
		{
			it->second.lastAccess = now;
			return ProcessResult { std::ref(it->second.process), ProcessChange::Existing };
		}

		auto process = ReadProcess(pid);
		if (!process)
		{
			return std::nullopt;
		}

		auto res = m_cache.emplace(pid,
			CachedProcess
			{
				*process,
				now
			}
		);

		return ProcessResult{ std::ref(res.first->second.process), ProcessChange::Started };
	}

	void ProcessManager::CleanUp(std::chrono::seconds maxAge)
	{
		auto now = std::chrono::steady_clock::now();

		std::lock_guard lock(m_mutex);

		for (auto it = m_cache.begin(); it != m_cache.end();)
		{
			auto process = ReadProcess(it->second.process.pid);
			// PID Re-use check
			if (!process || it->second.process.start_time != process->start_time)
			{
				it = m_cache.erase(it);
				continue;
			}

			if (now - it->second.lastAccess > maxAge)
			{
				it = m_cache.erase(it);
				continue;
			}
			++it;
		}
	}

	bool ProcessManager::IsPidDirectory(const std::string& name) const
	{
		return !name.empty() && std::all_of(name.begin(), name.end(), ::isdigit);
	}

	std::optional<ProcessInfo> ProcessManager::ReadProcess(const uint32_t pid) const
	{
		std::string proc_path = "/proc/" + std::to_string(pid);
		if (!fs::exists(proc_path))
		{
			return std::nullopt;
		}

		ProcessInfo info{};
		info.pid = pid;

		// Read process name
		{
			std::ifstream comm_file(proc_path + "/comm");
			if (comm_file)
			{
				std::getline(comm_file, info.name);
			}
		}
		
		// Read process start time
		{
			std::ifstream stat_file(proc_path + "/stat");
			if (stat_file)
			{
				std::string line;
				std::getline(stat_file, line);
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

