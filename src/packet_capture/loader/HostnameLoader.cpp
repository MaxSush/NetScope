#include "HostnameLoader.h"

#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <ifaddrs.h>
#include <net/if.h>

#include <logger/Logger.h>
#include <bpf/bpf_hostname_event.h>
#include <parser/DnsResponseParser.h>
#include <parser/SniParser.h>

namespace netscope
{
	HostnameLoader::HostnameLoader(hostname::DnsCache& cache)
		:m_cache(cache)
	{
	}

	bool HostnameLoader::Load()
	{
		m_bpfObject = bpf_object__open_file("hostname_capture.bpf.o", nullptr);
		if (!m_bpfObject)
		{
			LOG_ERROR("Failed to open hostname_capture.bpf.o");
			return false;
		}

		if (bpf_object__load(m_bpfObject))
		{
			LOG_ERROR("Failed to load hostname_capture BPF");
			return false;
		}

		if (!AttachTc())
		{
			LOG_ERROR("Failed to attach TC programs");
			return false;
		}

		bpf_map* map = bpf_object__find_map_by_name(m_bpfObject, "hostname_events");
		if (!map)
		{
			LOG_ERROR("Failed to find hostname_events map");
			return false;
		}

		m_ringBuffer = ring_buffer__new(bpf_map__fd(map), HandleEvent, this, nullptr);

		return m_ringBuffer != nullptr;
	}

	void HostnameLoader::Unload()
	{
		DetachTc();

		if (m_ringBuffer)
		{
			ring_buffer__free(m_ringBuffer);
			m_ringBuffer = nullptr;
		}

		if (m_bpfObject)
		{
			bpf_object__close(m_bpfObject);
			m_bpfObject = nullptr;
		}
	}

	void HostnameLoader::Poll()
	{
		if (m_ringBuffer)
		{
			ring_buffer__poll(m_ringBuffer, 200);
		}
	}

	bool HostnameLoader::AttachTc()
	{
		bpf_program* dns_prog = bpf_object__find_program_by_name(m_bpfObject, "tc_dns_ingress");
		bpf_program* sni_prog = bpf_object__find_program_by_name(m_bpfObject, "tc_sni_egress");

		if (!dns_prog || !sni_prog)
		{
			LOG_ERROR("TC programs not found in BPF object");
			return false;
		}

		int dns_fd = bpf_program__fd(dns_prog);
		int sni_fd = bpf_program__fd(sni_prog);

		struct ifaddrs* ifaddr;
		if (getifaddrs(&ifaddr) == -1)
		{
			return false;
		}

		std::set<int> attached;

		for (auto* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
		{
			if (!ifa->ifa_name)
			{
				continue;
			}

			std::string name = ifa->ifa_name;
			int ifindex = if_nametoindex(name.c_str());
			if (ifindex == 0 || attached.count(ifindex))
			{
				continue;
			}

			if (name == "lo" || // not attached to loopback interface
				name.substr(0, 6) == "docker" ||
				name.substr(0, 4) == "veth" ||
				name.substr(0, 3) == "br-" ||
				name.substr(0, 5) == "virbr")
			{
				continue;
			}

			LIBBPF_OPTS(bpf_tc_hook, hook, .ifindex = ifindex, .attach_point = BPF_TC_INGRESS);
			bpf_tc_hook_create(&hook);

			LIBBPF_OPTS(bpf_tc_opts, dns_opts, .prog_fd = dns_fd);
			hook.attach_point = BPF_TC_INGRESS;
			bool ingress_ok = (bpf_tc_attach(&hook, &dns_opts) == 0);

			bool egress_ok = false;
			//if (name != "lo")
			//{
				LIBBPF_OPTS(bpf_tc_opts, sni_opts, .prog_fd = sni_fd);
				hook.attach_point = BPF_TC_EGRESS;
				egress_ok = (bpf_tc_attach(&hook, &sni_opts) == 0);
			//}

			if (ingress_ok || egress_ok)
			{
				m_attachments.push_back({ ifindex, ingress_ok, egress_ok });
				LOG_INFO("TC attached to {} (dns={}, sni={})", name, ingress_ok, egress_ok);
			}

			attached.insert(ifindex);
		}

		freeifaddrs(ifaddr);
		return !m_attachments.empty();
	}

	void HostnameLoader::DetachTc()
	{
		for (auto& att : m_attachments)
		{
			LIBBPF_OPTS(bpf_tc_hook, hook, .ifindex = att.ifindex);
			if (att.ingress)
			{
				hook.attach_point = BPF_TC_INGRESS;
				bpf_tc_hook_destroy(&hook);
			}
			if (att.egress)
			{
				hook.attach_point = BPF_TC_EGRESS;
				bpf_tc_hook_destroy(&hook);
			}
		}
		m_attachments.clear();
	}

	int HostnameLoader::HandleEvent(void* ctx, void* data, size_t size)
	{
		auto* self = static_cast<HostnameLoader*>(ctx);
		auto* event = static_cast<bpf_hostname_event*>(data);

		if (event->event_type == HOSTNAME_EVENT_DNS)
		{
			auto mapping = hostname::DnsResponseParser::Parse(event->payload, event->payload_len);
			if (mapping)
			{
				auto ttl = std::chrono::seconds(mapping->ttl);
				for (const auto& ip : mapping->addresses)
				{
					self->m_cache.Insert(ip, mapping->queryDomain, hostname::ResolutionSource::PASSIVE_DNS, ttl);
				}
			}
		}
		else if (event->event_type == HOSTNAME_EVENT_TLS)
		{
			auto hostname = hostname::SniParser::Parse(event->payload, event->payload_len);
			if (hostname)
			{
				flow::IPAddress ip;
				if (event->family == FAMILY_IPV4)
				{
					ip = flow::IPAddress::IPv4(event->dst_ip.ipv4);
				}
				else
				{
					std::array<uint8_t, 16> addr;
					std::memcpy(addr.data(), event->dst_ip.ipv6, 16);
					ip = flow::IPAddress::IPv6(addr);
				}

				self->m_cache.Insert(ip, *hostname, hostname::ResolutionSource::SNI, std::chrono::seconds(86400));
			}
		}
		return 0;
	}
}