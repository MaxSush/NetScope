#include "NetworkCoordinator.h"

namespace netscope::processing
{
	NetworkCoordinator::NetworkCoordinator()
	{
	}

	void NetworkCoordinator::Coordinate(const ProcessingContext& context)
	{
        if (context.flow.has_value())
        {
            if (context.flow->IsCreated())
            {
                if (context.process.has_value()) 
                {
                    m_flowCreatedBus.Publish({ context.flow->GetSnapshot(), context.process->GetProcess() });
                }
                else 
                {
                    process::ProcessInfo unknownProc{};
                    unknownProc.pid = context.input.pid;
                    unknownProc.name = "Unknown";
                    m_flowCreatedBus.Publish({ context.flow->GetSnapshot(), unknownProc });
                }
            }

            if (context.flow->IsUpdated())
            {
                m_flowUpdatedBus.Publish({ context.flow->GetSnapshot() });
            }
        }
		
	}
}