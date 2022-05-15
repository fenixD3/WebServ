#pragma once

#include <string>
#include <map>
#include <deque>

class VirtualServer;

class IConfig
{
public:
	typedef std::map<std::string, std::string> physical_ports_ip_cont;

public:
	virtual void Process() = 0;

	virtual const physical_ports_ip_cont& GetPhysicalEndpoints() const = 0;
	virtual const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const = 0;
};

/// Test output
/*class FakeConfig : public IConfig
{
private:
	physical_ports_ip_cont m_PhysicalPortsToIp;

	/// The first server in deque mut be a default
	std::map<std::string, std::deque<VirtualServer*>> m_PortToVirtualServers;

public:
	FakeConfig();

	const physical_ports_ip_cont& GetPhysicalEndpoints() const override;
	const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const override;
};*/
