#pragma once

#include "IConfig.h"
#include "VirtualServer.h"

#include <fstream>

namespace config
{

static const std::string DefaultConfig = "./confs/default.conf";
static const std::string SpacePattern = "\n\t\r ";

class BlockExtractor
{
public:
	static const char BlockStartPattern = '{';
	static const char BlockEndPattern = '}';
	static const char InstructionEndPattern = ';';
	static const char CommentPattern = '#';

private:
	static const std::deque<std::string> AvailableBlock;

	static std::deque<std::string> GetAvailableBlock();

	std::fstream m_ConfigFile;

public:
	BlockExtractor(const std::string& file_path);

	virtual ~BlockExtractor();

	bool TryExtract(std::map<std::string, std::deque<std::string> >& out_block_content);

private:
	std::string ExtractBlockName();
};

class Config : public IConfig
{
private:
	BlockExtractor m_BlockExtractor;

	physical_ports_ip_cont m_PhysicalPortsToIp;
	std::map<std::string, std::deque<VirtualServer*> > m_PortToVirtualServers; /// The first server in deque mut be a default

public:
	Config(const std::string& file_path);

	virtual void Process();

	virtual const physical_ports_ip_cont& GetPhysicalEndpoints() const;
	virtual const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const;

private:
	std::string ParseServer(const std::deque<std::string>& block_instructions, VirtualServerBuilder& vs_builder);
	void ParseLocations(const std::deque<std::string>& block_instructions, VirtualServerBuilder& vs_builder);
	bool IsExistSameServer(const std::string& port, const std::string& serv_name);
};

std::string ExtractLogicPart(std::istream& where_extract, const char delimiter = '\n');
void Trim(std::string& out_line);

}

