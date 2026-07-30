#pragma once

#include "storage/FlowTable.h"
#include <pipeline/IContextProcessor.h>
#include <lifecycle/ILifecycleProcessor.h>

namespace netscope::flow
{
	class FlowManager : public processing::IContextProcessor, public processing::ILifecycleProcessor
	{
	public:
		FlowManager() = default;

		void Process(processing::ProcessingContext& context) override;

		void Process(processing::LifecycleContext& context) override;
		void CleanUpArchived() override;

		FlowResult Update(const NetworkEvent& event);

		std::vector<FlowSnapshot> GetSnapshot() const;

		void Clear();

	private:
		FlowTable m_Table;

	};
}