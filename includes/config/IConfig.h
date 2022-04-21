#pragma once

#include <string>
#include <map>
#include <deque>

class IConfig
{
public:
	typedef std::deque<std::string> physical_ports_cont;

public:
	virtual const physical_ports_cont& GetPhysicalPorts() const = 0;
};

class FakeConfig : public IConfig
{
private:
	physical_ports_cont m_PhysicalPorts = {
		"8080",
		"8181"
	};
	std::map<std::string, size_t> m_PortToVirtualServers = {
		{"8080", 2},
		{"8181", 1}
	};

public:
	const physical_ports_cont& GetPhysicalPorts() const override;
};
