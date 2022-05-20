#include "Worker.h"
#include <sys/stat.h>
#include "HttpResponseBuilder.h"

#include "CgiWorker.h"


HttpResponse Worker::ProcessRequest(HttpRequest& request, const Location& location) {
	if (!location.IsMethodAllowed(ToHttpMethod(request.GetMethod()))) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(405, location);
	}
	if (request.request_address.find("..") != std::string::npos) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(403, location);
	}

	if (false) { // if (location.is_cgi_path(request.request_address))
		return ProcessCGIRequest(request, location);
	}

	switch (ToHttpMethod(request.GetMethod()))
	{
	case GET:
		return HttpGet(request, location);
		break;
	case HEAD:
		return HttpHead(request, location);
		break;
	case POST:
		return HttpPost(request, location);
		break;
	default:
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(501, location);
		break;
	}
}

HttpResponse Worker::ProcessCGIRequest(HttpRequest& request, const Location& location) {
	std::string request_address = request.request_address;
	std::string cgi_script_path = ".";
	while (request_address.size()) {
		size_t first_slash = request_address.find('/');
		std::string file_or_dir;
		if (first_slash != std::string::npos) {
			file_or_dir = request_address.substr(0, first_slash);
		} else {
			file_or_dir = request_address;
		}
		if (!IsFileExist(cgi_script_path + "/" + file_or_dir)) {
			break;
		}
		if (first_slash == std::string::npos) {
			break;
		}
		request_address = request_address.substr(0, first_slash + 1);
		cgi_script_path += "/" + file_or_dir;
	}

	CgiWorker cgi_worker;
	std::string responce_body = cgi_worker.executeCgi();

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




HttpResponse Worker::HttpGet(HttpRequest& request, const Location& location) {
	std::string file_path = ResolvePagePath(request.request_address, location.root, location.index_file);

	if (!IsFileExist(file_path)) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, location);
	}

	std::string file_content = ReadFile(file_path);

	HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse(file_content, 200);
	// response.SetHeader("тип файла", "текст");

	return response;
}

HttpResponse Worker::HttpHead(HttpRequest& request, const Location& location) {
	HttpResponse response = HttpGet(request, location);
	response.body = "";
	return response;
}


HttpResponse Worker::HttpPost(HttpRequest& request, const Location& location) {
	// POST метод можно обработать только с помощью CGI
	return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, location);
}


HttpResponse Worker::HttpDelete(HttpRequest& request, const Location& location) {
	std::string file_path = ResolvePagePath(request.request_address, location.root, location.index_file);

	if (IsFileExist(file_path)) {
		remove(file_path.c_str());
	}
	return HttpResponseBuilder::GetInstance().CreateResponse("{\"success\":\"true\"}", 200);
}
