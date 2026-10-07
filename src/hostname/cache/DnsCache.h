#pragma once

#include <string>
#include <optional>
#include <list>
#include <unordered_map>
#include <mutex>

#include <model/IPAddress.h>
#include <FlowHash.h>

namespace netscope::hostname
{
	enum class ResolutionSource : uint8_t
	{
		SNI = 0,
		PASSIVE_DNS,
		REVERSE_DNS
	};

	struct CacheEntry
	{
		std::string hostname;
		ResolutionSource source;
		std::chrono::steady_clock::time_point expiresAt;
	};

	class DnsCache
	{
	public:
		explicit DnsCache(size_t maxEntries = 8192);

		void Insert(const flow::IPAddress& ip, const std::string hostname, ResolutionSource source, std::chrono::seconds ttl);
		std::optional<std::string> Lookup(const flow::IPAddress& ip) const;
		size_t Size() const;

	private:
		size_t m_maxEntries;

		using LruList = std::list<flow::IPAddress>;
		mutable LruList m_lruList;

		struct InternalEntry
		{
			CacheEntry entry;
			LruList::iterator lruIt;
		};

		mutable std::unordered_map<flow::IPAddress, InternalEntry> m_cache;
		mutable std::mutex m_mutex;

		void EvictLru();
		void Touch(typename std::unordered_map<flow::IPAddress, InternalEntry>::iterator it) const;
	};
}