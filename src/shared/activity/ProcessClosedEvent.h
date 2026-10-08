#pragma once

#include <model/ProcessInfo.h>

namespace netscope::processing
{
	struct ProcessClosedEvent
	{
		process::ProcessInfo processInfo;
	};
}