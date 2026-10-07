#include "DnsCache.h"

namespace netscope::hostname
{
	DnsCache::DnsCache(size_t maxEntries)
		: m_maxEntries(std::max<size_t>(maxEntries, 1))
	{
	}

	void DnsCache::Insert(const flow::IPAddress& ip, const std::string hostname, ResolutionSource source, std::chrono::seconds ttl)
	{
		std::lock_guard lock(m_mutex);

		auto expiresAt = std::chrono::steady_clock::now() + ttl;
		auto it = m_cache.find(ip);

		if (it != m_cache.end())
		{
			bool isExpired = std::chrono::steady_clock::now() > it->second.entry.expiresAt;

			// overwrite only if new source has equal or higher priority (lower enum value)
			if (source <= it->second.entry.source || isExpired)
			{
				it->second.entry = { std::move(hostname), source, expiresAt };
				Touch(it);
			}
			return;
		}

		while (m_cache.size() >= m_maxEntries)
		{
			EvictLru();
		}

		m_lruList.push_front(ip);
		m_cache.try_emplace(ip, InternalEntry{ { std::move(hostname), source, expiresAt }, m_lruList.begin() });
	}

	std::optional<std::string> DnsCache::Lookup(const flow::IPAddress& ip) const
	{
		std::lock_guard lock(m_mutex);

		auto it = m_cache.find(ip);
		if (it == m_cache.end())
		{
			return std::nullopt;
		}

		if (std::chrono::steady_clock::now() > it->second.entry.expiresAt)
		{
			m_lruList.erase(it->second.lruIt);
			m_cache.erase(it);
			return std::nullopt;
		}

		Touch(it);

		return it->second.entry.hostname;
	}

	size_t DnsCache::Size() const
	{
		std::lock_guard lock(m_mutex);
		return m_cache.size();
	}

	void DnsCache::EvictLru()
	{
		if (m_lruList.empty())
		{
			return;
		}

		auto& oldest = m_lruList.back();
		m_cache.erase(oldest);
		m_lruList.pop_back();
	}

	void DnsCache::Touch(typename std::unordered_map<flow::IPAddress, InternalEntry>::iterator it) const
	{
		m_lruList.splice(m_lruList.begin(), m_lruList, it->second.lruIt);
	}
}