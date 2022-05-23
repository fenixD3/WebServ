#pragma once

#include <deque>

#include "VirtualServer.h"
#include "HttpRequest.h"

class PhysicalServer
{
public:
	std::deque<raii_ptr<VirtualServer> > m_VirtualServers;

public:
	PhysicalServer(const std::deque<VirtualServer*>& servers);

	std::string ProcessRequest(HttpRequest *request);
};
