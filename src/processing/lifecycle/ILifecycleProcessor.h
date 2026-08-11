#pragma once
#include <time/TimeUtils.h>

namespace netscope::processing
{
	struct LifecycleContext;

	class ILifecycleProcessor
	{
	public:
		virtual ~ILifecycleProcessor() = default;

		virtual void Process(LifecycleContext& context) = 0;

		virtual void CleanUpExpired(Steady_TimePoint now) = 0;
	};
}