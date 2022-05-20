#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <HttpRequest.h>

class Location
{
public:
    std::string pattern = "/"; 
    std::vector<HttpMethod> allowed_methods;
    std::string root = ".";
    std::string index_file = "index.html";
    std::map<int, std::string> error_pages; // = {std::make_pair(404, "./default_pages/errors/404.html")};
    std::string default_error_page_dir = "./default_pages/errors/";
    
    bool IsMethodAllowed(HttpMethod method) const;
    bool IsUrlMatchLocation(std::string url);
    std::string GetErrorPage(int for_code) const;
	bool operator<(const Location& other_location) const;
};