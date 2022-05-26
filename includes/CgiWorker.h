#pragma once

#include <string>
#include "HttpRequest.h"
#include <map>

class CgiWorker {
	std::map<std::string, std::string> env;
	char** create_env(std::string cgi_script_path, std::string request_address, HttpRequest* request_pointer);
public:
	std::string executeCgi(std::string cgi_script_path, std::string request_address, HttpRequest* request);
	CgiWorker();
};

class CgiException : public std::exception {};