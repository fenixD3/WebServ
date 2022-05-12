#pragma once

#include "IConfig.h"

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

	std::fstream& m_ConfigFile;

public:
	BlockExtractor(std::fstream& file);

	bool TryExtract(std::map<std::string, std::deque<std::string> >& out_block_content);
};

/*class InstructionExtractor
{
private:
//	static const char InstructionEndPattern = ';';

	std::fstream& m_ConfigFile;

public:
	InstructionExtractor(std::ifstream& file);
};*/

class Config : public IConfig
{
private:
	std::fstream m_ConfigFile;
	BlockExtractor m_BlockExtractor;

	physical_ports_cont m_PhysicalPorts;
	std::map<std::string, std::deque<VirtualServer*> > m_PortToVirtualServers; /// The first server in deque mut be a default

public:
	Config(const std::string& file_path);
	~Config();

	void Process() override;

	const physical_ports_cont& GetPhysicalPorts() const override;
	const std::deque<VirtualServer*>& GetServersByPort(const std::string& port) const override;
};

std::string ExtractLogicPart(std::istream& where_extract, const char delimiter = '\n');
void Trim(std::string& out_line);
void DeleteComment(std::string& out_line);

}

