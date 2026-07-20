#pragma once
#include <context/ProcessingContext.h>

namespace netscope::processing
{
	class IContextProcessor
	{
	public:
		virtual ~IContextProcessor() = default;

		virtual bool CanProcess([[maybe_unused]]const ProcessingContext& context) const
		{
			return true;
		}


		virtual void Process(ProcessingContext& context) = 0;
	};
}