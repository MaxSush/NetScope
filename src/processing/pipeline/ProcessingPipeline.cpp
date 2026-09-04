#include <coordinator/NetworkCoordinator.h>
#include "ProcessingPipeline.h"


namespace netscope::processing
{
	ProcessingPipeline::ProcessingPipeline(NetworkCoordinator& coordinator)
		:
		m_coordinator(coordinator)
	{
	}

	void ProcessingPipeline::AddStage(IContextProcessor& processor)
	{
		m_processors.push_back(&processor);
	}

	void ProcessingPipeline::Process(const NetworkEvent& event)
	{
		ProcessingContext context(event);

		for (auto* processor : m_processors)
		{
			processor->Process(context);
			if (context.drop)
			{
				return;
			}
		}

		m_coordinator.Coordinate(context);
	}

}
