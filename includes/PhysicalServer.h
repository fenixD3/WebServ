#pragma once

#include "VirtualServer.h"
#include "HttpRequest.h"

struct CacheElement {
	HttpRequest request;
	std::string response;
	CacheElement(HttpRequest& request, const std::string& response): request(request), response(response) {}
};

class Cache {
	std::vector<CacheElement> cache;
	static const int max_cache_size;
	static const int max_body_size;
	
	public:
	std::string get_cached_request(const HttpRequest& request);
	void add_to_cache(HttpRequest& request, const std::string& responce);

};

class PhysicalServer
{
public:
	std::map<std::string, raii_ptr<VirtualServer> > m_NameToVirtualServers;
	Cache cache;
public:
	PhysicalServer(const std::deque<VirtualServer*>& servers);
	VirtualServer* ResolveUrlToServer(HttpRequest *request);
	std::string ProcessRequest(HttpRequest *request);
};
