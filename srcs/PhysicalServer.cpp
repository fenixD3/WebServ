#include "PhysicalServer.h"

PhysicalServer::PhysicalServer(const std::deque<VirtualServer*>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		m_NameToVirtualServers.insert(std::make_pair(servers[i]->m_ServerName, raii_ptr<VirtualServer>(servers[i])));
	}
}

std::string PhysicalServer::ProcessRequest(HttpRequest *request)
{
	/// It should be a logic here
	return "";
}
