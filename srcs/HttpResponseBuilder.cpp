
#include "HttpResponseBuilder.h"
#include <fstream>
#include <sys/time.h>
#include <sstream>
#include <fstream>

std::string ReadFile(std::string path) {
	std::ifstream file(path);

	std::ifstream m_stream(path);

	if (!file.good()) {
        return "";
	}
	std::stringstream ss;
	ss << m_stream.rdbuf();
	return ss.str();
}

std::string HttpResponseBuilder::GetFormatedDate() {
	char buf[200];
	struct timeval date;
	gettimeofday(&date, NULL);
	struct tm tm = *gmtime(&date.tv_sec);
	strftime(buf, 200, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	return std::string(buf);
}


ResponseHeader HttpResponseBuilder::CreateBaseHeaders() {
	ResponseHeader headers;
	headers["Server"] = "Webserv42/v1.0";
	headers["Date"] = GetFormatedDate();
	return headers;
}

HttpResponse HttpResponseBuilder::CreateResponse(std::string body, int code) { //}, const Location& location) {
	HttpResponse response;
	response.code = code;
	response.body = body;
	response.header = CreateBaseHeaders();
	response.header["Content-Length"] = std::to_string(response.body.size());
	response.header["Content-Type"] = "text/html";
	response.status = std::string("Ok");
	return response;
}

HttpResponse HttpResponseBuilder::CreateErrorResponse(int code, const VirtualServer* virtual_server) {
	HttpResponse response;
	response.code = code;
	response.body = ReadFile(virtual_server->GetErrorPage(code));
	response.header = CreateBaseHeaders();
	response.header["Content-Length"] = std::to_string(response.body.size());
	response.header["Content-Type"] = "text/html";
	response.status = std::string("Error");
	return response;
}

