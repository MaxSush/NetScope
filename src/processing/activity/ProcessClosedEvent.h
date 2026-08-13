#pragma once

#include <ProcessInfo.h>

namespace netscope::processing
{
	struct ProcessClosedEvent
	{
		process::ProcessInfo processInfo;
	};
}