#pragma once
#include <functional>
#include "ProcessInfo.h"

namespace netscope::process
{
	enum class ProcessChange
	{
		EXISTING,
		STARTED,
		EXITED
	};

	struct ProcessResult
	{
		std::reference_wrapper<const ProcessInfo> process;
		ProcessChange change;

		const ProcessInfo& GetProcess() const 
		{
			return process.get();
		}

		bool IsStarted() const
		{
			return change == ProcessChange::STARTED;
		}

		bool IsExisting() const
		{
			return change == ProcessChange::EXISTING;
		}

		bool IsExited() const
		{
			return change == ProcessChange::EXITED;
		}
	};
}