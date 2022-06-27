#include "Worker.h"
#include <sys/stat.h>
#include "HttpResponseBuilder.h"
#include "CgiWorker.h"
#include <fstream>
#include <sstream>
#include <dirent.h>

Worker::Worker() {}
Worker::~Worker() {}

HttpResponse Worker::ProcessRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	if (!location || !virtual_server || !location->IsMethodAllowed(request->GetMethod())) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(405, virtual_server);
	}
    if (location->redirect.size()) {
        return RedirectResponse(location->redirect);
    }
	if (request->GetPath().find("..") != std::string::npos) {
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(403, virtual_server);
	}
    if (location->client_max_body_size >= 0 && static_cast<long long>(request->GetBody().size()) > location->client_max_body_size) {
        return HttpResponseBuilder::GetInstance().CreateErrorResponse(413, virtual_server);
    }

	if (location->IsCgiPath(request->GetPath())) {
		std::cout << "Start CGI" << std::endl;
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
    case PUT:
        return HttpPost(request, virtual_server, location);
        break;
    case DELETE:
        return HttpDelete(request, virtual_server, location);
        break;
	default:
		return HttpResponseBuilder::GetInstance().CreateErrorResponse(501, virtual_server);
		break;
	}
}

HttpResponse Worker::ProcessCGIRequest(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
//	std::string request_address = request->GetPath().substr(location->uri.size());
    std::string request_address = request->GetPath();
	std::string cgi_script_path = location->cgi_script;
//	while (request_address.size()) {
//		size_t first_slash = request_address.find('/');
//        if (first_slash == 0) {
//            request_address = request_address.substr(first_slash + 1);
//            continue;
//        }
//		std::string file_or_dir;
//		if (first_slash != std::string::npos) {
//			file_or_dir = request_address.substr(0, first_slash);
//		} else {
//			file_or_dir = request_address;
//		}
//		if (!IsPathExist(cgi_script_path + "/" + file_or_dir)) {
//            break;
//        }
//		request_address = request_address.substr(0, first_slash + 1);
//		cgi_script_path += "/" + file_or_dir;
//        if (IsFileExist(cgi_script_path) || first_slash == std::string::npos) {
//            break;
//        }
//	}
    if (!IsFileExist(cgi_script_path)) {
        return HttpResponseBuilder::GetInstance().CreateErrorResponse(500, virtual_server);
    }
	CgiWorker cgi_worker;
	std::string responce_body = cgi_worker.executeCgi(cgi_script_path, request_address, request);
	int status = 200;
    std::string header_block = "";
    if (responce_body.find("\r\n\r\n") != std::string::npos) {
        header_block = responce_body.substr(0, responce_body.find("\r\n\r\n"));
        responce_body = responce_body.substr(responce_body.find("\r\n\r\n") + 4);
    }

    std::map<std::string, std::string> headers;

    while(header_block.size()) {
        size_t line_end = header_block.find("\r\n");
        std::string line;
        if (line_end == std::string::npos) {
            line = header_block;
            header_block = "";
        } else {
            line = header_block.substr(0, line_end);
            header_block = header_block.substr(line_end + 2);
        }
        size_t sep = line.find(": ");
        if (sep == std::string::npos) {
            break;
        }
        std::string key = line.substr(0, sep);
        std::string value = line.substr(sep + 2);
        if (key == "Status") {
            std::istringstream(value) >> status;
        } else {
            headers[key] = value;
        }
    }
	HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse(responce_body, status);
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        response.SetHeader(it->first, it->second);
	}

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

bool Worker::IsPathExist(std::string path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

bool Worker::IsDirExist(std::string file_path) {
    struct stat buffer;
    if (!(stat(file_path.c_str(), &buffer) == 0)) {
        return false;
    }
    return buffer.st_mode & S_IFDIR;
}

bool Worker::IsFileExist(std::string file_path) {
    struct stat buffer;
    if (!(stat(file_path.c_str(), &buffer) == 0)) {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

std::string ListDir(std::string dir_path) {
    std::string dirs_string;
    struct dirent *entry;
    DIR *dir = opendir(dir_path.c_str());
    if (dir == NULL) {
        return dirs_string;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            dirs_string += "/";
        }
        dirs_string += entry->d_name;
        dirs_string += "<br/>\n";
    }
    closedir(dir);
    return dirs_string;
}


HttpResponse Worker::HttpGet(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
	std::string file_path = ResolvePagePath(request->GetPath(), virtual_server, location);

	if (!IsFileExist(file_path)) {
        std::string dir_path = location->path + "/" + request->GetPath().substr(location->uri.size());
        if (IsDirExist(dir_path)) {
            return HttpResponseBuilder::GetInstance().CreateResponse(ListDir(dir_path), 404);
        } else {
            return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, virtual_server);
        }
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

std::string ExtractFileName(HttpRequest* request) {
    if (!request->count("Content-Disposition")) {
        return "";
    }
    std::string filename_part = request->at("Content-Disposition");
    if (filename_part.find("filename") == std::string::npos) {
        return "";
    }
    filename_part = filename_part.substr(filename_part.find("filename") + 9);
    filename_part = filename_part.substr(filename_part.find("\"") + 1);
    filename_part = filename_part.substr(0, filename_part.find("\""));
    std::cout << filename_part << std::endl;
    return filename_part;
}


std::string get_file_path(std::string location_path, std::string location_url, std::string request_path, std::string file_name) {
    std::string file_path;
    if (location_url.size() > 1) {
        request_path = request_path.substr(location_url.size());
    }
    if (!ends_with(request_path, "/")) {
        file_path = location_path + request_path;
    } else if (file_name.size()) {
        file_path = location_path + request_path + "/" + file_name;
    } else {
        file_path = location_path + request_path + "/ " + "loaded_file.tmp";
    }
    return file_path;
}

std::string get_file_dir(std::string location_path, std::string location_url, std::string request_path) {
    std::string dir = location_path + request_path;
    if (location_url.size() > 1) {
        request_path = request_path.substr(location_url.size());
    }
    if (!ends_with(request_path, "/")) {
        size_t sep = request_path.rfind("/");
        dir = location_path + request_path.substr(0, sep);
    }
    return dir;
}

HttpResponse Worker::HttpPost(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
    std::string file = ExtractFileName(request);
    if (file.empty()) {
        file = "default_file";
    }
    std::string request_path = request->GetPath();
    std::string file_path = get_file_path(location->path, location->uri, request_path, file);
    std::string dir = get_file_dir(location->path, location->uri, request_path);

    if (!IsDirExist(dir)) {
        return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, virtual_server);
    }

    std::ofstream file_stram;
    file_stram.open(file_path);
    file_stram << std::string(request->GetBody().begin(), request->GetBody().end());
    file_stram.close();
    HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse("{status: ok}", 201);

    return response;
}

HttpResponse Worker::HttpDelete(HttpRequest* request, const VirtualServer* virtual_server, const VirtualServer::UriProps* location) {
    std::string file = ExtractFileName(request);
    std::string request_path = request->GetPath();
    std::string file_path = get_file_path(location->path, location->uri, request_path, file);
    std::string dir = get_file_dir(location->path, location->uri, request_path);

    if (!IsDirExist(dir) || !IsFileExist(file_path)) {
        return HttpResponseBuilder::GetInstance().CreateErrorResponse(404, virtual_server);
    }

    std::remove(file_path.c_str());

    HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse("{status: ok}", 201);

    return response;
}

HttpResponse Worker::RedirectResponse(std::string redirect_page) {
    HttpResponse response = HttpResponseBuilder::GetInstance().CreateResponse("Moved Permanently", 301);
    response.SetHeader("Location", redirect_page);
    return response;
}
