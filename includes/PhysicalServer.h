#pragma once

#include "VirtualServer.h"
#include "HttpRequest.h"

class PhysicalServer
{
public:
	std::map<std::string, VirtualServer* > m_NameToVirtualServers;

public:
	PhysicalServer(const std::deque<VirtualServer*>& servers);
	VirtualServer* v_server;
	VirtualServer* ResolveUrlToServer(HttpRequest *request);
	std::string ProcessRequest(HttpRequest *request);
};
