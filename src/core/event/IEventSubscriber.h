#pragma once

namespace netscope::core
{
	template<typename T>
	class IEventSubscriber
	{
	public:
		virtual ~IEventSubscriber() = default;

		virtual void HandleEvent(const T& event) = 0;

	};
}