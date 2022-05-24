#pragma once

#include "VirtualServer.h"
#include "HttpRequest.h"

class PhysicalServer
{
public:
	std::map<std::string, raii_ptr<VirtualServer> > m_NameToVirtualServers;

public:
	PhysicalServer(const std::deque<VirtualServer*>& servers);

	std::string ProcessRequest(HttpRequest *request);
};
