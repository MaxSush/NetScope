#include "NetworkCoordinator.h"

#include <context/LifecycleContext.h>
#include <storage/FlowResult.h>
#include <logger/Logger.h>


namespace netscope::processing
{
	NetworkCoordinator::NetworkCoordinator()
	{
	}

	void NetworkCoordinator::Coordinate(const ProcessingContext& context)
	{
		// Single-flow result (PACKET, CONNECTION_CLOSED)
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

		// Multi-flow result (PROCESS_EXITED)
		for (const auto& closedFlow : context.closedFlows)
		{
			m_flowClosedBus.Publish({ closedFlow.GetSnapshot() });
		}

		// Process exit event
		if (context.process.has_value() && context.process->IsExited())
		{
			m_processClosedBus.Publish({ context.process->GetProcess() });
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
		for (const auto& processResult : context.exitedProcess)
		{
			m_processClosedBus.Publish({ processResult.process });
		}
    }

}