#pragma once

namespace netscope::processing
{
	struct LifecycleContext;

	class ILifecycleProcessor
	{
	public:
		virtual ~ILifecycleProcessor() = default;

		virtual void Process(LifecycleContext& context) = 0;

		virtual void CleanUpArchived() = 0;
	};
}