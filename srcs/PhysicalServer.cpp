#include "PhysicalServer.h"

PhysicalServer::PhysicalServer(const std::deque<VirtualServer*>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		m_VirtualServers.push_back(raii_ptr<VirtualServer>(servers[i]));
	}
}

std::string PhysicalServer::ProcessRequest(HttpRequest *request)
{
	return "";
}
