#pragma once

#include <queue>
#include <unordered_set>
#include <mutex>
#include <condition_variable>

#include <cache/DnsCache.h>

namespace netscope::hostname
{
	class RdnsResolver
	{
	public:
		explicit RdnsResolver(DnsCache& cache);

		void Enqueue(const flow::IPAddress& ip);

		void Run();
		void Stop();

	private:
		DnsCache& m_cache;

		static constexpr size_t MAX_QUEUE_SIZE = 1024;
		std::queue<flow::IPAddress> m_queue;

		std::unordered_set<flow::IPAddress> m_pending;
		std::mutex m_mutex;
		std::condition_variable m_cv;
		bool m_stopped = false;

		std::optional<std::string> Resolve(const flow::IPAddress& ip);
	};
}