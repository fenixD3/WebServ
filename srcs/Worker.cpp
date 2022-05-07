#include "Worker.h"
#include <sys/stat.h>

#include "HttpResponseBuilder.h"



HttpResponse Worker::ProcessRequest(const HttpRequest& request, const Location& location) {
	if (!location.IsMethodAllowed(request.http_method)) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(405, location);
	}

	switch (request.http_method)
	{
	case GET:
		return HttpGet(request, location);
		break;
	case HEAD:
		return HttpHead(request, location);
		break;
	default:
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(501, location);
		break;
	}
}


std::string ResolvePagePath(std::string request_address, std::string root, std::string default_file) {
	std::string path = root; //+ request_address + default_file;

	if (root.back() == '/' && request_address.front() == '/') {
		path += request_address.substr(1);
	} else {
		path += request_address;
	}
	if (path.back() != '/') {
		return path;
	}
	path += default_file;
	return path;
}

bool Worker::IsFileExist(std::string file_path) {
	struct stat buffer;   
	return (stat (file_path.c_str(), &buffer) == 0); 
}




HttpResponse Worker::HttpGet(const HttpRequest& request, const Location& location) {
	if (request.request_address.find("..") != std::string::npos) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(403, location);
	}

	std::string file_path = ResolvePagePath(request.request_address, location.root, location.index_file);

	if (!IsFileExist(file_path)) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, location);
	}

	std::string file_content = ReadFile(file_path);

	HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse(file_content, 200);
	// response.SetHeader("тип файла", "текст");

	return response;
}

HttpResponse Worker::HttpHead(const HttpRequest& request, const Location& location) {
	HttpResponse response = HttpGet(request, location);
	response.body = "";
	return response;
}

