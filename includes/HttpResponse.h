#pragma once

#include "VirtualServer.h"

typedef std::map<type_tag, type_value> ResponseHeader;

std::string ReadFile(std::string path);

class HttpResponse {
public:
	int code;
	std::string message;
	ResponseHeader header;
	std::string body;
	std::string status;

	std::string get_as_raw_string();
};

