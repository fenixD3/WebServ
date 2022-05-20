#pragma once

#include "HttpResponse.h"
#include "Location.h"
class HttpResponseBuilder {
public:

	static HttpResponseBuilder& GetInstance() {
		static HttpResponseBuilder instance;

		return instance;
	}
	HttpResponse CreateErrorResponse(int code, const Location& location);
	HttpResponse CreateResponse(std::string body, int code); //, const Location& location);


private:
	std::map<std::string, std::string> CreateBaseHeaders();
	std::string GetFormatedDate();

	HttpResponseBuilder() {}
	HttpResponseBuilder(const HttpResponseBuilder&);
	void operator=(const HttpResponseBuilder&);
};

