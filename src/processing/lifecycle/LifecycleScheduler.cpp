#include "LifecycleScheduler.h"
#include <context/LifecycleContext.h>

namespace netscope::processing
{
	LifecycleScheduler::LifecycleScheduler(NetworkCoordinator& coordinator)
		:	m_coordinator(coordinator)
	{
	}

	void LifecycleScheduler::AddProcessor(ILifecycleProcessor& processor)
	{
		m_processors.push_back(processor);
	}

	void LifecycleScheduler::Process()
	{
		LifecycleContext context(Steady_Clock::now());

		for (auto processor : m_processors)
		{
			processor.get().Process(context);
		}

		m_coordinator.Coordinate(context);

		for (auto processor : m_processors)
		{
			processor.get().CleanUpArchived();
		}
	}
}