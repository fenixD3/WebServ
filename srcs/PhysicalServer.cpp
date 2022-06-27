#include "PhysicalServer.h"
#include "Worker.h"
#include <fstream>

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
    for (std::map<std::string, raii_ptr<VirtualServer> >::reverse_iterator it = m_NameToVirtualServers.rbegin();
                                                                it != m_NameToVirtualServers.rend(); ++it) {
	 	if (request->at("Host").rfind(it->first, 0) == 0) {
             return it->second.get();
         }
	 }
    return m_NameToVirtualServers.begin()->second.get();
}

std::string PhysicalServer::ProcessRequest(HttpRequest *request)
{
	std::string cached = cache.get_cached_request(*request);
	if (cached.size()) {
		return cached;
	}

	Worker worker;

	VirtualServer* virtual_server = ResolveUrlToServer(request);
	VirtualServer::UriProps* location = virtual_server->GetLocationForUrl(request->GetPath());
	std::string responce = worker.ProcessRequest(request, virtual_server, location).get_as_raw_string();
	cache.add_to_cache(*request, responce);
    
	return responce;
}

const int Cache::max_cache_size = 3;
const int Cache::max_body_size = 250;

std::string Cache::get_cached_request(const HttpRequest& request) {
	for (size_t i = 0; i < cache.size(); ++i) {
		if (cache[i].request == request) {
			return cache[i].response;
		}
	}
	return "";
}

void Cache::add_to_cache(HttpRequest& request, const std::string& responce) {
	if (request.GetBody().size() > max_body_size || responce.size() > max_body_size) {
		return;
	}
	cache.push_back(CacheElement(request, responce));
	if (cache.size() > max_cache_size) {
		std::rotate(cache.begin(), cache.begin() + 1, cache.end());
		cache.pop_back();
	}
}