#include "config/Config.h"

#include <iostream>

const std::string Config::SpacePattern = "\n\t\r ";

Config::Config() {}

void Config::SetFile(const std::string& file_path)
{
	m_ConfigFile.open(file_path);
	if (!m_ConfigFile.is_open())
	{
		std::cerr << "Config file doesn't exist" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Config::Process()
{
	for (std::string line; std::getline(m_ConfigFile, line);)
	{
//		std::string block_name = ExtractLogicPart(Config::BlockStart);
		Trim(line);
//		if (block_name.empty() || block_name.find("server") == std::string::npos)
//		{
//			std::cerr << "Config file block isn't a server" << std::endl;
//			exit(EXIT_FAILURE);
//		}

	}
}

const IConfig::physical_ports_cont& Config::GetPhysicalPorts() const
{
	return m_PhysicalPorts;
}

const std::deque<VirtualServer*>& Config::GetServersByPort(const std::string& port) const
{
	return m_PortToVirtualServers.at(port);
}

Config::~Config()
{
	m_ConfigFile.close();
}

std::string Config::ExtractLogicPart(const char delimiter)
{
	std::string result;
	std::getline(m_ConfigFile, result, delimiter);
	return result;
}

void Config::Trim(std::string& out_line)
{
	size_t trimmed_end = out_line.find_first_not_of(Config::SpacePattern);
	if (trimmed_end != 0)
	{
		out_line.erase(0, trimmed_end);
	}
}
