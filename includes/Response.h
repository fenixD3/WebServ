
#include "VirtualServer.h"

typedef std::map<type_tag, type_value> ResponseHeader;

std::string read_file(std::string path);

class HttpResponse {
public:
	int code;
	std::string message;
	ResponseHeader header;
	std::string body;
	std::string status;

	std::string get_as_raw_string();
};

class HttpResponseBuilder {
public:

	static HttpResponseBuilder& GetInstance() {
		static HttpResponseBuilder instance;

		return instance;
	}
	HttpResponse CreateErrorResponse(int code, const Location& location);
	HttpResponse CreateResponse(std::string body, int code, const Location& location);

	std::map<std::string, std::string> CreateBaseHeaders();
private:

	HttpResponseBuilder();
	HttpResponseBuilder(const HttpResponseBuilder&);
	void operator=(const HttpResponseBuilder&);
};




