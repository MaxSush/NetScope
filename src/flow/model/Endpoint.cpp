#include "Endpoint.h"

namespace netscope::flow
{
	bool Endpoint::operator==(const Endpoint& other) const
	{
		return address == other.address && port == other.port;
	}

	bool Endpoint::operator<(const Endpoint& other) const
	{
		if (address == other.address)
		{
			return port < other.port;
		}
		return address < other.address;
	}

	std::string Endpoint::ToString() const
	{
		return address.ToString() + ":" + std::to_string(port);
	}
}