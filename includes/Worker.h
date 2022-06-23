#pragma once

#include "VirtualServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"


class Worker
{
private:
	HttpResponse HttpGet(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
	HttpResponse HttpHead(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
	HttpResponse HttpPost(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
//	HttpResponse HttpDelete(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
	HttpResponse ProcessCGIRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
	HttpResponse HttpDelete(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);
	bool IsFileExist(std::string file_path);
    bool IsPathExist(std::string path);
    bool IsDirExist(std::string file_path);
public:
	Worker();
	~Worker();

	HttpResponse ProcessRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location);


};


