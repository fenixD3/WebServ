#include "Worker.h"
#include "Response.h"





HttpResponse Worker::ProcessRequest(const HttpRequest& request, const Location& location) {
	if (!location.IsMethodAllowed(request.http_method)) {
		return HttpResponseBuilder.GetInstance().CreateErrorResponse(405, location);
	}

	switch (request.http_method)
	{
	case GET:
		return HttpGet(request, location);
		break;
	
	default:
		return HttpResponseBuilder.GetInstance().CreateErrorResponse(501, location);
		break;
	}
}


std::string ResolvePagePath(std::string request_address, std::string root, std::string default_file) {

}

HttpResponse Worker::HttpGet(const HttpRequest& request, const Location& location) {
	if (request.request_address.find("..") != std::string.npos) {
		HttpResponseBuilder.GetInstance().CreateErrorResponse(403, location);
	}

	std::string file_path = ResolvePagePath(request.request_address, location.root, location.index_file);

	if (!IsFileExist(file_path)) {
		HttpResponseBuilder.GetInstance().CreateErrorResponse(404, location);
	}

	std::string file_content = ReadFile(file_path);

	HttpResponse responce = HttpResponseBuilder.GetInstance().CreateResponse(file_content, 200, location);
	// responce.SetHeader("тип файла", "текст");

	return responce;
}

