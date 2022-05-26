#include "Worker.h"
#include <sys/stat.h>
#include "HttpResponseBuilder.h"

#include "CgiWorker.h"


Worker::Worker() {}
Worker::~Worker() {}

HttpResponse Worker::ProcessRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	if (!location || !location->IsMethodAllowed(request->GetMethod())) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(405, virtual_server);
	}
	if (request->GetPath().find("..") != std::string::npos) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(403, virtual_server);
	}

	if (false) { // if (location.is_cgi_path(request.request_address))
		return ProcessCGIRequest(request, virtual_server, location);
	}

	switch (ToHttpMethod(request->GetMethod()))
	{
	case GET:
		return HttpGet(request, virtual_server, location);
		break;
	case HEAD:
		return HttpHead(request, virtual_server, location);
		break;
	case POST:
		return HttpPost(request, virtual_server, location);
		break;
	default:
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(501, virtual_server);
		break;
	}
}

HttpResponse Worker::ProcessCGIRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	std::string request_address = request->GetPath();
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
	std::string responce_body = cgi_worker.executeCgi(cgi_script_path, request_address, request);
	size_t body_start = responce_body.find("\n\n");
	size_t content_type_colon = responce_body.find("\n\n");
	if (body_start == std::string::npos || content_type_colon == std::string::npos) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(500, virtual_server);
	}
	HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse(responce_body.substr(body_start), 200);
	response.SetHeader("Content-Type", responce_body.substr(content_type_colon, body_start));
	return response;
}

std::string ResolvePagePath(std::string request_address, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
    if (request_address == "") {
        request_address = "/";
    }
	std::string path = location->path + "/" + request_address.substr(location->uri.size());
    struct stat s;
    if( stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            path += "/" + virtual_server->m_StandardRoutes.at(LocationNames::Index);
        }
    }

	return path;
}

bool Worker::IsFileExist(std::string file_path) {
	struct stat buffer;   
	return (stat (file_path.c_str(), &buffer) == 0); 
}




HttpResponse Worker::HttpGet(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	std::string file_path = ResolvePagePath(request->GetPath(), virtual_server, location);

	if (!IsFileExist(file_path)) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, virtual_server);
	}

	std::string file_content = ReadFile(file_path);

	HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse(file_content, 200);
	// response.SetHeader("тип файла", "текст");

	return response;
}

HttpResponse Worker::HttpHead(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	HttpResponse response = HttpGet(request, virtual_server, location);
	response.body = "";
	return response;
}


HttpResponse Worker::HttpPost(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	// POST метод можно обработать только с помощью CGI
	return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, virtual_server);
}


//HttpResponse Worker::HttpDelete(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
//	std::string file_path = ResolvePagePath(request->GetPath(), virtual_server, location);
//
//	if (IsFileExist(file_path)) {
//		remove(file_path.c_str());
//	}
//	return HttpResponseBuilder::GetInstance().CreateResponse("{\"success\":\"true\"}", 200);
//}
