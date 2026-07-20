#pragma once
#include <vector>

#include "IContextProcessor.h"

namespace netscope::processing
{
	class NetworkCoordinator;

	class ProcessingPipeline
	{
	public:
		explicit ProcessingPipeline(NetworkCoordinator& coordinator);

		void AddStage(IContextProcessor& processor);

		void Process(const NetworkEvent& event);

	private:
		std::vector<IContextProcessor*> m_processors;
		
		NetworkCoordinator& m_coordinator;
	};
}