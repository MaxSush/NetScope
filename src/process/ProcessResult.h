#pragma once
#include <functional>
#include "ProcessInfo.h"

namespace netscope::process
{
	enum class ProcessChange
	{
		Existing,
		Started,
		Exited
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
			return change == ProcessChange::Started;
		}

		bool IsExisting() const
		{
			return change == ProcessChange::Existing;
		}

		bool IsExited() const
		{
			return change == ProcessChange::Existing;
		}
	};
}