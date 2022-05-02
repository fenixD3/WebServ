
#include "Response.h"
#include <fstream>


std::string read_file(std::string path) {
	std::ifstream file(path);

	return std::string((std::istreambuf_iterator<char>(file)),
                  (std::istreambuf_iterator<char>()));
}

ResponseHeader HttpResponseBuilder::CreateBaseHeaders() {
	
}



HttpResponse HttpResponseBuilder::CreateErrorResponse(int code, const Location& location) {
	HttpResponse responce;
	responce.body = read_file(location.GetErrorPage(code));
	responce.code = code;
	responce.header = CreateBaseHeaders();
	responce.status = std::string("Error");
}

