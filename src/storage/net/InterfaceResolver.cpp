#include "InterfaceResolver.h"

#include <net/if.h>
#include <ifaddrs.h>
#include <cstring>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <logger/Logger.h>

namespace netscope::storage
{
	InterfaceResolver::InterfaceResolver()
	{
		Refresh();
	}

	std::string InterfaceResolver::Resolve(uint32_t ifindex) const
	{
		if (ifindex == 0)
		{
			return "unknown";
		}

		auto it = m_cache.find(ifindex);
		if (it != m_cache.end())
		{
			return it->second;
		}

		char name[IF_NAMESIZE];
		if (if_indextoname(ifindex, name))
		{
			return std::string(name);
		}

		return "unknown";
	}

	void InterfaceResolver::Refresh()
	{
		m_cache.clear();
		
		struct if_nameindex* interfaces = if_nameindex();
		if (!interfaces)
		{
			return;
		}

		for (auto* it = interfaces; it->if_index != 0 || it->if_name != nullptr; ++it)
		{
			m_cache[it->if_index] = it->if_name;
		}

		if_freenameindex(interfaces);
	}

	uint32_t InterfaceResolver::ResolveRoute(const flow::IPAddress& destination)
	{
		auto it = m_routeCache.find(destination);
		if (it != m_routeCache.end())
		{
			return it->second;
		}

		int af = (destination.IsIPv4()) ? AF_INET : AF_INET6;
		int fd = socket(af, SOCK_DGRAM, 0);
		if (fd < 0)
		{
			return 0;
		}

		std::string ipStr = destination.ToString();
		uint32_t result = 0;
		if (af == AF_INET)
		{
			struct sockaddr_in addr {};
			addr.sin_family = AF_INET;
			addr.sin_port = htons(80);
			inet_pton(AF_INET, ipStr.c_str(), &addr.sin_addr);

			if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
			{
				struct sockaddr_in local {};
				socklen_t len = sizeof(local);
				if (getsockname(fd, (struct sockaddr*)&local, &len) == 0)
				{
					struct ifaddrs* ifaddr;
					if (getifaddrs(&ifaddr) == 0)
					{
						for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next)
						{
							if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET)
							{
								continue;
							}
							auto* sa = (struct sockaddr_in*)ifa->ifa_addr;
							if (sa->sin_addr.s_addr == local.sin_addr.s_addr)
							{
								result = if_nametoindex(ifa->ifa_name);
								break;
							}
						}
						freeifaddrs(ifaddr);
					}
				}
			}
		}
		else
		{
			struct sockaddr_in6 addr {};
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htons(80);
			inet_pton(AF_INET6, ipStr.c_str(), &addr.sin6_addr);

			if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
			{
				struct sockaddr_in6 local {};
				socklen_t len = sizeof(local);
				if (getsockname(fd, (struct sockaddr*)&local, &len) == 0)
				{
					struct ifaddrs* ifaddr;
					if (getifaddrs(&ifaddr) == 0)
					{
						for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next)
						{
							if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET6)
							{
								continue;
							}
							auto* sa6 = (struct sockaddr_in6*)ifa->ifa_addr;
							if (memcmp(&sa6->sin6_addr, &local.sin6_addr, 16) == 0)
							{
								result = if_nametoindex(ifa->ifa_name);
								break;
							}
						}
						freeifaddrs(ifaddr);
					}
				}
			}
		}

		close(fd);

		m_routeCache[destination] = result;
		return result;
	}
}