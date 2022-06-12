#include "PhysicalServer.h"
#include "Worker.h"

PhysicalServer::PhysicalServer(const std::deque<VirtualServer*>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		m_NameToVirtualServers.insert(std::make_pair(servers[i]->m_ServerName, raii_ptr<VirtualServer>(servers[i])));
	}
}

VirtualServer* PhysicalServer::ResolveUrlToServer(HttpRequest *request) {
    if (request->find("Host") == request->end()) {
        return m_NameToVirtualServers.begin()->second.get();
    }
    for (std::map<std::string, raii_ptr<VirtualServer> >::iterator it = m_NameToVirtualServers.begin();
                                                                it != m_NameToVirtualServers.end(); ++it) {
	 	if (request->at("Host").rfind(it->first, 0) == 0) {
             return it->second.get();
         }
	 }
    return m_NameToVirtualServers.begin()->second.get();
}

std::string PhysicalServer::ProcessRequest(HttpRequest *request)
{
	Worker worker;

	VirtualServer* virtual_server = ResolveUrlToServer(request);
	VirtualServer::UriProps* location = virtual_server->GetLocationForUrl(request->GetPath());
	std::string responce = worker.ProcessRequest(request, virtual_server, location).get_as_raw_string();
	return responce;
}
