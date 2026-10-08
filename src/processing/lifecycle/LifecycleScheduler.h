#pragma once

#include <coordinator/NetworkCoordinator.h>
#include <pipeline/ILifecycleProcessor.h>
#include <atomic>

namespace netscope::processing
{
	class LifecycleScheduler
	{
	public:
		explicit LifecycleScheduler(NetworkCoordinator& coordinator);

		void AddProcessor(ILifecycleProcessor& processor);

		void Process();

	private:
		std::vector<std::reference_wrapper<ILifecycleProcessor>> m_processors;

		NetworkCoordinator& m_coordinator;
	};
}