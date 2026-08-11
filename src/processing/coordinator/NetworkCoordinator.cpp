#include "NetworkCoordinator.h"

#include <context/LifecycleContext.h>
#include <storage/FlowResult.h>
#include <iostream>


namespace netscope::processing
{
	NetworkCoordinator::NetworkCoordinator()
	{
	}

	void NetworkCoordinator::Coordinate(const ProcessingContext& context)
	{
        if (context.flow.has_value())
        {
            switch (context.flow->change)
            {
			case flow::FlowChange::CREATED:
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
				break;
			case flow::FlowChange::UPDATED:
			case flow::FlowChange::RESUMED:
				m_flowUpdatedBus.Publish({ context.flow->GetSnapshot() });
				break;
			case flow::FlowChange::CLOSED:
				m_flowClosedBus.Publish({ context.flow->GetSnapshot() });
				break;
			default:
				break;
            }
        }
	}

    void NetworkCoordinator::Coordinate(const LifecycleContext& context)
    {
		for (const auto& flow : context.flowChanges)
		{
            switch (flow.change)
            {
            case flow::FlowChange::IDLE:
				m_flowUpdatedBus.Publish({ flow.GetSnapshot() });
				break;
			case flow::FlowChange::CLOSED:
				m_flowClosedBus.Publish({ flow.GetSnapshot() });
				break;
			default:
                break;
            }
		}
    }

}