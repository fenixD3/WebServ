#pragma once

#include "IConfig.h"

#include <fstream>

static const std::string DefaultConfig = "./confs/default.conf";

class Config : public IConfig
{
private:
	static const char BlockStart = '{';
	static const char BlockEnd = '}';
	static const char InstructionEnd = ';';
	static const std::string SpacePattern;

	std::ifstream m_ConfigFile;

	physical_ports_cont m_PhysicalPorts;

	/// The first server in deque mut be a default
	std::map<std::string, std::deque<VirtualServer*> > m_PortToVirtualServers;

public:
	Config();
	~Config();

	void SetFile(const std::string& file_path);

	void Process() override;

	const physical_ports_cont& GetPhysicalPorts() const override;
	const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const override;

private:
	std::string ExtractLogicPart(const char delimiter);
	void Trim(std::string& out_line);
};
