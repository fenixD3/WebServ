#pragma once
#include <map>
#include <string>

typedef std::string type_tag;
typedef std::string type_value;
typedef std::map<type_tag, type_value> ResponseHeader;

std::string ReadFile(std::string path);

class HttpResponse {
public:
	int code;
	std::string message;
	ResponseHeader header;
	std::string body;
	std::string status;

	void SetHeader(std::string name, std::string value);
	std::string get_as_raw_string();
};

