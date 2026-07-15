#pragma once

#include "IPAddress.h"

namespace netscope::flow
{
	struct Endpoint
	{
		friend struct std::hash<Endpoint>;

		IPAddress address;
		uint16_t port;

		bool operator==(const Endpoint& other) const;
		bool operator<(const Endpoint& other) const;

		std::string ToString() const;
	};
}

namespace std
{
	template<>
	struct hash<netscope::flow::Endpoint>;
}
