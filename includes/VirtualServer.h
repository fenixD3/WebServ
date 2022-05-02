#pragma once

#include <vector>
#include <string>
#include <map>
#include <deque>
#include <iostream>

#include "raii_ptr.h"

/*
From commit 64422faae3a0d80ba619b044163f2b5e2d564825 - initial for request parcer
*/
enum HttpMethod
 {
     GET,
     POST,
     DELETE,
     //PUT,
     OTHER
 };
typedef std::string type_tag;
typedef std::string type_value;
typedef std::map<type_tag, type_value> RequestHeader;
typedef RequestHeader::iterator header_iterator;
typedef HttpMethod e_http_method;
class HttpRequest : public RequestHeader
{
    private:
        size_t m_size;
        std::vector<char> m_body; // maybe std::array due no dynamic change
        e_http_method http_method;
        std::string request_address;
    public:
        HttpRequest(
            size_t size,
            std::vector<char>& body,
            e_http_method http_method)
        : m_size(m_size)
        , m_body(body)
        {
            // TODO Add all headers tags
            this->emplace("METHOD", ToString(http_method));
        }
};
///  END commit 64422faae3a0d80ba619b044163f2b5e2d564825 - initial for request parcer



class Location
{
public:
    std::string pattern; 
    std::vector<HttpMethod> allowed_methods;
    std::string root = ".";
    std::string index_file = "index.html";
    std::map<int, std::string> error_pages; // = {std::make_pair(404, "./default_pages/errors/404.html")};
    std::string default_error_page = "./default_pages/errors/404.html";
    
    bool IsMethodAllowed(HttpMethod method) const;
    bool IsUrlMatchLocation(std::string url);
    std::string GetErrorPage(int for_code) const;
};


class VirtualServer
{
	friend class VirtualServerBuilder;
public:
	struct UriProps
	{
		std::deque<std::string> excepted_methods;
		std::string path;
	};
public:
    std::string m_ServerName;
	size_t m_BodyLimit;
	std::map<std::string, UriProps> m_UriToProperties;
	std::map<std::string, std::string> m_ErrorRoutes;
	std::map<std::string, std::string> m_StandardRoutes;
    std::vector<Location> locations;

public:
	VirtualServer();

	/// Test output
	/*friend std::ostream& operator<<(std::ostream& out, const VirtualServer& vs)
	{
		out << "Server name: " << vs.m_ServerName << ", Body Limit: " << vs.m_BodyLimit << std::endl;
		for (auto& [uri, props] : vs.m_UriToProperties)
		{
			out << "Uri: " << uri << " with path: " << props.path << ' ';
			if (props.excepted_methods.empty())
			{
				out << "Excepted methods there aren't" << std::endl;
			}
			else
			{
				out << "Excepted methods: ";
				for (auto& i : props.excepted_methods)
				{
					out << i << ' ';
				}
				out << std::endl;
			}
		}

		for (auto& [error, path] : vs.m_ErrorRoutes)
		{
			out << "Error: " << error <<" has path: " << path << std::endl;
		}

		for (auto& [name, route] : vs.m_StandardRoutes)
		{
			out << name <<" has route: " << route << std::endl;
		}
		return out;
	}*/
};

struct LocationNames
{
	static const std::string Index;
	static const std::string Cgi;
	static const std::string Upload;
	static const std::string UriPath;
	static const std::string ExceptedMethods;
};

class LocationBuilder
{
private:
	std::string m_RootPath;
	std::map<std::string, std::string> m_StandardRoutes;
	std::map<std::string, std::string> m_Errors;
	std::map<std::string, std::map<std::string, std::string> > m_UriToProperties;

public:
	void AddRoot(const std::string& root);
	void AddIndex(const std::string& index_page);
	void AddError(const std::string& error_code, const std::string& error_page);
	void AddCgi(const std::string& cgi_path);
	void AddUpload(const std::string& upload_path);
	void AddUriProperty(const std::string& uri, const std::string& prop_name, const std::string& prop_value);
	void BuildAllRoutes();

	std::map<std::string, std::string> GetStandardRoutes() const;
	std::map<std::string, std::string> GetErrors() const;
	std::map<std::string, std::map<std::string, std::string> > GetUriProperties() const;
};

class VirtualServerBuilder
{
private:
    raii_ptr<VirtualServer> m_VS;
	LocationBuilder m_LocationBuilder;

public:
	VirtualServerBuilder();
    operator VirtualServer*();

    void AddServerName(const std::string& serv_name = "");
	void AddBodyLimit(const size_t body_size);
	void AddRoot(const std::string& root);
	void AddIndex(const std::string& index_page);
	void AddError(const std::string& error_code, const std::string& error_page);
	void AddCgi(const std::string& cgi_path);
	void AddUpload(const std::string& upload_path);
	void AddUriProperty(const std::string& uri, const std::string& prop_name, const std::string& prop_value);
	void BuildAllRoutes();
};
