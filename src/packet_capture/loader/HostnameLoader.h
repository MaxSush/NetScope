#pragma once

#include <vector>
#include <set>

#include <cache/DnsCache.h>

struct bpf_object;
struct ring_buffer;

namespace netscope
{
	class HostnameLoader
	{
	public:
		explicit HostnameLoader(hostname::DnsCache& cache);

		bool Load();
		void Unload();

		void Poll();

	private:
		hostname::DnsCache& m_cache;

		bpf_object* m_bpfObject = nullptr;
		ring_buffer* m_ringBuffer = nullptr;

		struct TcAttachment
		{
			int ifindex;
			bool ingress;
			bool egress;
		};

		std::vector<TcAttachment> m_attachments;

		bool AttachTc();
		void DetachTc();

		static int HandleEvent(void* ctx, void* data, size_t size);
	};
}