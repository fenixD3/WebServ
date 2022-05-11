#pragma once

#include <string>
#include <map>
#include <deque>

class VirtualServer;

class IConfig
{
public:
	typedef std::deque<std::string> physical_ports_cont;

public:
	virtual void Process() = 0;

	virtual const physical_ports_cont& GetPhysicalPorts() const = 0;
	virtual const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const = 0;
};

/*class FakeConfig : public IConfig
{
private:
	physical_ports_cont m_PhysicalPorts;

	/// The first server in deque mut be a default
	std::map<std::string, std::deque<VirtualServer*>> m_PortToVirtualServers;

public:
	FakeConfig();

	const physical_ports_cont& GetPhysicalPorts() const override;
	const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const override;
};*/
