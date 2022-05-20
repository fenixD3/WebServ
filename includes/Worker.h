#pragma once

#include "VirtualServer.h"
#include "HttpResponse.h"


class Worker
{
private:
	HttpResponse HttpGet(HttpRequest& request, const Location& location);
	HttpResponse HttpHead(HttpRequest& request, const Location& location);
	HttpResponse HttpPost(HttpRequest& request, const Location& location);
	HttpResponse HttpDelete(HttpRequest& request, const Location& location);
	HttpResponse ProcessCGIRequest(HttpRequest& request, const Location& location);

	bool IsFileExist(std::string file_path);
public:
	Worker();
	~Worker();

	HttpResponse ProcessRequest(HttpRequest& request, const Location& location);

};


