#include "config/Config.h"
#include "VirtualServer.h"

#include <iostream>
#include <sstream>

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

BlockExtractor::BlockExtractor(const std::string& file_path)
	: m_ConfigFile(file_path)
{
	if (!m_ConfigFile.is_open())
	{
		std::cerr << "Config file doesn't exist" << std::endl;
		exit(EXIT_FAILURE);
	}
}

BlockExtractor::~BlockExtractor()
{
	m_ConfigFile.close();
}

bool BlockExtractor::TryExtract(std::map<std::string, std::deque<std::string> >& out_block_content)
{
	std::string block_name = ExtractBlockName();
	if (block_name.empty())
	{
		return false;
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
		if (instruction_end == std::string::npos)
		{
			std::cerr << "Instruction must be ended ';'" << std::endl;
			exit(EXIT_FAILURE);
		}
		out_block_content[block_name].push_back(line.erase(instruction_end));
	}

	return true;
}

std::string BlockExtractor::ExtractBlockName()
{
	std::string block_name;
	while (m_ConfigFile)
	{
		block_name = ExtractLogicPart(m_ConfigFile);
		if (block_name.empty() || block_name.front() == '#')
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
	return block_name;
}

Config::Config(const std::string& file_path)
	: m_BlockExtractor(file_path)
{}

void Config::Process()
{
	std::map<std::string, std::deque<std::string> > block_instructions;
	while (m_BlockExtractor.TryExtract(block_instructions))
	{
		VirtualServerBuilder vs_builder;
		std::string port = ParseServer(block_instructions["server"], vs_builder);
		ParseLocations(block_instructions["location"], vs_builder);
		vs_builder.BuildAllRoutes();
		if (IsExistSameServer(port, vs_builder.GetServerName()))
		{
			std::cerr << "There are two equals server" << std::endl;
			exit(EXIT_FAILURE);
		}
		m_PortToVirtualServers[port].push_back(vs_builder);

		/// Test output
		/*std::cout << "Virtual Server:" << std::endl << *m_PortToVirtualServers[port].back()
				  << "On port: " << port << " and ip: " << m_PhysicalPortsToIp[port] << std::endl << std::string(100, '-') << std::endl;*/

		block_instructions.clear();
	}
}

std::string Config::ParseServer(const std::deque<std::string>& block_instructions, VirtualServerBuilder& vs_builder)
{
	std::string port;
	for (size_t i = 0; i < block_instructions.size(); ++i)
	{
		std::istringstream instruction(block_instructions[i]);
		std::string name;
		instruction >> name;
		if (name == "listen")
		{
			std::string value, ip;
			instruction >> value;
			size_t delim_pos = value.find(':');
			if (delim_pos != std::string::npos)
			{
				ip = value.substr(0, delim_pos);
				port = value.substr(delim_pos + 1);
			}
			else
			{
				port = value;
			}
			if (m_PhysicalPortsToIp.count(port) && m_PhysicalPortsToIp[port] != ip)
			{
				std::cerr << "Ip has been already existed for port " << port <<std::endl;
				exit(EXIT_FAILURE);
			}
			m_PhysicalPortsToIp[port] = ip;
		}
		else if (name == "server_name")
		{
			std::string serv_name;
			instruction >> serv_name;
			if (serv_name == "_")
			{
				serv_name = "";
			}
			vs_builder.AddServerName(serv_name);
		}
		else if (name == "client_max_body_size")
		{
			size_t limit = 0;
			instruction >> limit;
			vs_builder.AddBodyLimit(limit);
		}
		else if (name == "root")
		{
			std::string root;
			instruction >> root;
			if (root.empty())
			{
				std::cerr << "Root must be at server block" << std::endl;
				exit(EXIT_FAILURE);
			}
			vs_builder.AddRoot(root);
		}
		else if (name == "index")
		{
			std::string index_page;
			instruction >> index_page;
			if (index_page.empty())
			{
				std::cerr << "Index must has a value" << std::endl;
				exit(EXIT_FAILURE);
			}
			vs_builder.AddIndex(index_page);
		}
		else if (name == "error_page")
		{
			std::string error_code, error_page;
			instruction >> error_code >> error_page;
			if (error_code.empty() || error_page.empty())
			{
				std::cerr << "Error page must contain an error_code and path to the pahe" << std::endl;
				exit(EXIT_FAILURE);
			}
			vs_builder.AddError(error_code, error_page);
		}
		else if (name == "cgi_path")
		{
			std::string cgi_path;
			instruction >> cgi_path;
			if (cgi_path.empty())
			{
				std::cerr << "Cgi path must has a value" << std::endl;
				exit(EXIT_FAILURE);
			}
			vs_builder.AddCgi(cgi_path);
		}
		else if (name == "upload")
		{
			std::string upload_path;
			instruction >> upload_path;
			if (upload_path.empty())
			{
				std::cerr << "Upload must has a value" << std::endl;
				exit(EXIT_FAILURE);
			}
			vs_builder.AddUpload(upload_path);
		}
	}
	return port;
}

void Config::ParseLocations(const std::deque<std::string>& block_instructions, VirtualServerBuilder& vs_builder)
{
	std::string uri_value;
	{
		std::string uri;
		std::istringstream instruction(block_instructions[0]);
		instruction >> uri >> uri_value;
		if (uri != "uri")
		{
			std::cerr << "Uri must has at the first place in location block" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	for (size_t i = 1; i < block_instructions.size(); ++i)
	{
		std::istringstream instruction(block_instructions[i]);
		std::string name, value;
		instruction >> name >> value;
		if (name == "uri")
		{
			uri_value = value;
			continue;
		}
		vs_builder.AddUriProperty(uri_value, name, value);
	}
}

bool Config::IsExistSameServer(const std::string& port, const std::string& serv_name)
{
    if (!m_PortToVirtualServers.count(port)) {
        return false;
    }
	const std::deque<VirtualServer*>& servers = m_PortToVirtualServers.at(port);
	for (size_t i = 0; i < servers.size(); ++i)
	{
		if (servers[i]->m_ServerName == serv_name)
		{
			return true;
		}
	}
	return false;
}

const IConfig::physical_ports_ip_cont& Config::GetPhysicalEndpoints() const
{
	return m_PhysicalPortsToIp;
}

const std::deque<VirtualServer*>& Config::GetServersByPort(const std::string& port) const
{
	return m_PortToVirtualServers.at(port);
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

}
