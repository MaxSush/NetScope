#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>
#include <model/IPAddress.h>
#include <FlowHash.h>

namespace netscope::storage
{
	class InterfaceResolver
	{
	public:
		InterfaceResolver();

		std::string Resolve(uint32_t ifindex) const;
		void Refresh();
		uint32_t ResolveRoute(const flow::IPAddress& destination);

	private:
		std::unordered_map<uint32_t, std::string> m_cache;

		std::unordered_map<flow::IPAddress, uint32_t> m_routeCache;
	};
}