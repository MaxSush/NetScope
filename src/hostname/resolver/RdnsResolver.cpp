#include "RdnsResolver.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace netscope::hostname
{
	netscope::hostname::RdnsResolver::RdnsResolver(DnsCache& cache)
		: m_cache(cache)
	{
	}

	void RdnsResolver::Enqueue(const flow::IPAddress& ip)
	{
		if (m_cache.Lookup(ip).has_value())
		{
			return;
		}

		std::lock_guard lock(m_mutex);
		if (m_pending.count(ip))
		{
			return;
		}
		if (m_queue.size() >= MAX_QUEUE_SIZE)
		{
			return;
		}

		m_pending.insert(ip);
		m_queue.push(ip);
		m_cv.notify_one();
	}

	void RdnsResolver::Run()
	{
		flow::IPAddress ip;
		{
			std::unique_lock lock(m_mutex);
			m_cv.wait(lock, [this] {
					return !m_queue.empty() || m_stopped;
				});

			if (m_stopped)
			{
				return;
			}

			ip = m_queue.front();
			m_queue.pop();
		}

		auto hostname = Resolve(ip);

		if (hostname.has_value() && !hostname->empty())
		{
			m_cache.Insert(ip, *hostname, ResolutionSource::REVERSE_DNS, std::chrono::seconds(3600));
		}
		else
		{
			m_cache.Insert(ip, "", ResolutionSource::REVERSE_DNS, std::chrono::seconds(300));
		}

		{
			std::lock_guard lock(m_mutex);
			m_pending.erase(ip);
		}
	}

	void RdnsResolver::Stop()
	{
		std::lock_guard lock(m_mutex);
		m_stopped = true;
		m_cv.notify_all();
	}

	std::optional<std::string> RdnsResolver::Resolve(const flow::IPAddress& ip)
	{
		struct sockaddr_storage ss{};
		socklen_t ss_len;

		if (ip.IsIPv4())
		{
			auto* sa = reinterpret_cast<struct sockaddr_in*>(&ss);
			sa->sin_family = AF_INET;
			std::string ipStr = ip.ToString();
			if (inet_pton(AF_INET, ipStr.c_str(), &sa->sin_addr) != 1)
			{
				return std::nullopt;
			}
			ss_len = sizeof(struct sockaddr_in);

		}
		else
		{
			auto* sa6 = reinterpret_cast<struct sockaddr_in6*>(&ss);
			sa6->sin6_family = AF_INET6;
			std::string ipStr = ip.ToString();
			if (inet_pton(AF_INET6, ipStr.c_str(), &sa6->sin6_addr) != 1)
			{
				return std::nullopt;
			}
			ss_len = sizeof(struct sockaddr_in6);
		}


		char host[NI_MAXHOST];
		int result = getnameinfo(reinterpret_cast<struct sockaddr*>(&ss), ss_len, host, sizeof(host), nullptr, 0, NI_NAMEREQD);

		if (result == 0)
		{
			return std::string(host);
		}

		return std::nullopt;
	}
}