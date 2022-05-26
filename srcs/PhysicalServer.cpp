#include "PhysicalServer.h"
#include "Worker.h"
PhysicalServer::PhysicalServer(const std::deque<VirtualServer*>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		m_NameToVirtualServers.insert(std::make_pair(servers[i]->m_ServerName, servers[i]));
	}
}

VirtualServer* PhysicalServer::ResolveUrlToServer(HttpRequest *request) {
	// плохо понимаю, как находить нужный физический сервер для запроса
	// TODO
	return m_NameToVirtualServers.begin()->second;
	// return m_NameToVirtualServers["server"].get();
	// for (std::map<std::string, raii_ptr<VirtualServer>::iterator it : m_NameToVirtualServers) {
	// 	if (request->GetPath().rfind(pattern, 0) == it.second->)

	// }
}

std::string PhysicalServer::ProcessRequest(HttpRequest *request)
{
	Worker worker;

	VirtualServer* virtual_server = ResolveUrlToServer(request);
	VirtualServer::UriProps* location = virtual_server->GetLocationForUrl(request->GetPath());
	// if (!location) {
	// 	return worker.CreateErrorResponce(virtual_server, 404)
	// }

	std::string responce = worker.ProcessRequest(request, virtual_server, location).get_as_raw_string();
	return responce;
}
