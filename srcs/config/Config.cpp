#include "config/Config.h"

#include <iostream>

namespace config
{

const std::deque<std::string> BlockExtractor::AvailableBlock = BlockExtractor::GetAvailableBlock();

std::deque<std::string> BlockExtractor::GetAvailableBlock()
{
	std::deque<std::string> available_blocks;
	available_blocks.push_back("server");
	available_blocks.push_back("location");
	return available_blocks;
}

BlockExtractor::BlockExtractor(std::fstream& file)
	: m_ConfigFile(file)
{}

bool BlockExtractor::TryExtract(std::map<std::string, std::deque<std::string> >& out_block_content)
{
	std::string block_name;
	while (m_ConfigFile)
	{
		block_name = ExtractLogicPart(m_ConfigFile);
		if (block_name.empty())
		{
			continue;
		}
		if (block_name.find(BlockExtractor::BlockStartPattern) == std::string::npos)
		{
			std::cerr << "Block " << block_name << " has wrong syntax" << std::endl;
			exit(EXIT_FAILURE);
		}
		block_name.erase(block_name.find_first_of(SpacePattern));
		if (std::find(BlockExtractor::AvailableBlock.begin(), BlockExtractor::AvailableBlock.end(), block_name) == BlockExtractor::AvailableBlock.end())
		{
			std::cerr << "Block " << block_name << " isn't available" << std::endl;
			exit(EXIT_FAILURE);
		}
		break;
	}

	for (std::string line; m_ConfigFile;)
	{
		line = ExtractLogicPart(m_ConfigFile);
		if (line.empty() || line[0] == CommentPattern)
		{
			continue;
		}
		if (line.find(BlockExtractor::BlockEndPattern) != std::string::npos)
		{
			break;
		}
		if (line.find(BlockExtractor::BlockStartPattern) != std::string::npos)
		{
			if (line.find("server") != std::string::npos)
			{
				std::cerr << "Server block mustn't be at the another server block" << std::endl;
				exit(EXIT_FAILURE);
			}
			m_ConfigFile.putback('\n');
			for (std::string::const_reverse_iterator it = line.rbegin(); it != line.rend(); ++it)
			{
				m_ConfigFile.putback(*it);
			}
			TryExtract(out_block_content);
			continue;
		}
		size_t instruction_end = line.find(BlockExtractor::InstructionEndPattern);
		out_block_content[block_name].push_back(line.erase(instruction_end));
	}

	if (!m_ConfigFile)
	{
		return false;
	}
	return true;
}

Config::Config(const std::string& file_path)
	: m_ConfigFile(file_path)
	, m_BlockExtractor(m_ConfigFile)
{
	if (!m_ConfigFile.is_open())
	{
		std::cerr << "Config file doesn't exist" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Config::Process()
{
	std::map<std::string, std::deque<std::string> > block_instructions;
	while (m_BlockExtractor.TryExtract(block_instructions))
	{
		for (auto& [name, value] : block_instructions)
		{
			std::cout << name << std::endl;
			for (auto& i : value)
				std::cout << i << std::endl;
		}
		block_instructions.clear();
		std::cout << std::endl;
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

std::string ExtractLogicPart(std::istream& where_extract, const char delimiter)
{
	std::string result;
	std::getline(where_extract, result, delimiter);
	Trim(result);
	return result;
}

void Trim(std::string& out_line)
{
	size_t trimmed_end = out_line.find_first_not_of(SpacePattern);
	out_line.erase(0, trimmed_end);

	trimmed_end = out_line.find_last_not_of(SpacePattern);
	if (trimmed_end != std::string::npos)
	{
		out_line.erase(trimmed_end + 1);
	}
}

//void DeleteComment(std::string& out_line)
//{
//	size_t comment_start = out_line.find(BlockExtractor::CommentPattern);
//	if (comment_start != std::string::npos)
//	{
//		size_t comment_end = out_line.find('\n', comment_start);
//		out_line.erase(comment_start, )
//	}
//}

}
