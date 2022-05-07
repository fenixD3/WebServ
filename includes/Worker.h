#pragma once

#include "VirtualServer.h"
#include "HttpResponse.h"


class Worker
{
private:
	HttpResponse HttpGet(const HttpRequest& request, const Location& location);
	HttpResponse HttpHead(const HttpRequest& request, const Location& location);

	bool IsFileExist(std::string file_path);
public:
	Worker();
	~Worker();

	HttpResponse ProcessRequest(const HttpRequest& request, const Location& location);

};


