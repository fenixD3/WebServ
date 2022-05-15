#include "config/IConfig.h"
#include "VirtualServer.h"

/// Test output
/*FakeConfig::FakeConfig()
{
	m_PhysicalPortsToIp = {
			"8080",
			"8181"
	};

	m_PortToVirtualServers = {
		{"8080",
			{
				new VirtualServer("localhost", 1000000, "./errors_pages", {std::make_pair("FILES", "./files")}),
				new VirtualServer("google.com", 1000000, "./errors_pages", {std::make_pair("FILES", "./files")})
			}
			},
		{"8181",
			{
				new VirtualServer("localhost", 2000000, "./errors_pages_2", {std::make_pair("FILES", "./files")}),
			}
		}
	};
}

const IConfig::physical_ports_ip_cont& FakeConfig::GetPhysicalEndpoints() const
{
	return m_PhysicalPortsToIp;
}

const std::deque<VirtualServer*>& FakeConfig::GetServersByPort(const std::string& port) const
{
	return m_PortToVirtualServers.at(port);
}*/
