#include "VirtualServer.h"
#include <algorithm>

const std::string LocationNames::Index = "index";
const std::string LocationNames::Cgi = "cgi";
const std::string LocationNames::Upload = "upload";
const std::string LocationNames::UriPath = "path";
const std::string LocationNames::ExceptedMethods = "limit_except";

VirtualServer::VirtualServer() {}

VirtualServer::UriProps* VirtualServer::GetLocationForUrl(std::string url) {
	std::cout << this << " GetLocationForUrl " << &m_UriToProperties << std::endl;
//	std::map<std::string, UriProps>::iterator it;
  
	// return m_UriToProperties.begin()->second;
	std::map<std::string, UriProps>::reverse_iterator it;
    for (it = m_UriToProperties.rbegin(); it != m_UriToProperties.rend(); it++) {
        std::string url_a = url;
        std::string location_url = it->first;
       if (url.rfind(it->first, 0) == 0) {
		   return &it->second;
	   }
    }
	return NULL;
}

bool VirtualServer::UriProps::IsMethodAllowed(std::string method) const {
    return true;
    if (method.size()) {
        return true;
    }
}

VirtualServer::~VirtualServer() {
    std::cout << " DESTRUCTOR VirtualServer " << std::endl;
}

std::string VirtualServer::GetErrorPage(int code) const {
	if (m_ErrorRoutes.find(std::to_string(code)) != m_ErrorRoutes.end()) {
		return m_ErrorRoutes.at(std::to_string(code));
	}
	return m_ErrorRoutes.at(std::to_string(404)); // страница 404 есть у всех серверов? может слать просто любую страницу?
}

void LocationBuilder::AddRoot(const std::string& root)
{
	m_RootPath = root;
}

void LocationBuilder::AddIndex(const std::string& index_page)
{
	m_StandardRoutes[LocationNames::Index] = index_page;
}

void LocationBuilder::AddError(const std::string& error_code, const std::string& error_page)
{
	if (m_Errors.count(error_code))
	{
		std::cerr << "Error " << error_code << " has already contained" << std::endl;
		exit(EXIT_FAILURE);
	}
	m_Errors[error_code] = error_page;
}

void LocationBuilder::AddCgi(const std::string& cgi_path)
{
	m_StandardRoutes[LocationNames::Cgi] = cgi_path + '/';
}

void LocationBuilder::AddUpload(const std::string& upload_path)
{
	m_StandardRoutes[LocationNames::Upload] = upload_path + '/';
}

void LocationBuilder::AddUriProperty(const std::string& uri, const std::string& prop_name, const std::string& prop_value)
{
	m_UriToProperties[uri][prop_name] = prop_value;
}

void LocationBuilder::BuildAllRoutes()
{
	for (std::map<std::string, std::string>::iterator it = m_StandardRoutes.begin(); it != m_StandardRoutes.end(); ++it) {
        if (it->first == LocationNames::Index) {
            continue;
        }
		it->second = m_RootPath + it->second;
	}
	for (std::map<std::string, std::string>::iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
	{
		it->second = m_RootPath + it->second;
	}
	for (std::map<std::string, std::map<std::string, std::string> >::iterator it = m_UriToProperties.begin(); it != m_UriToProperties.end(); ++it)
	{
		it->second[LocationNames::UriPath] = m_RootPath + it->second[LocationNames::UriPath];
	}
}

std::map<std::string, std::string> LocationBuilder::GetStandardRoutes() const
{
	return m_StandardRoutes;
}

std::map<std::string, std::string> LocationBuilder::GetErrors() const
{
	return m_Errors;
}

std::map<std::string, std::map<std::string, std::string> > LocationBuilder::GetUriProperties() const
{
	return m_UriToProperties;
}

VirtualServerBuilder::VirtualServerBuilder()
	: m_VS(new VirtualServer)
{}

VirtualServerBuilder::operator VirtualServer*()
{
//	VirtualServer* result = m_VS.get();
//	m_VS = NULL;
	return m_VS;
}

void VirtualServerBuilder::AddServerName(const std::string& serv_name)
{
	m_VS->m_ServerName = serv_name;
}

void VirtualServerBuilder::AddBodyLimit(const size_t body_size)
{
	m_VS->m_BodyLimit = body_size;
}

void VirtualServerBuilder::AddRoot(const std::string& root)
{
	m_LocationBuilder.AddRoot(root);
}

void VirtualServerBuilder::AddIndex(const std::string& index_page)
{
	m_LocationBuilder.AddIndex(index_page);
}

void VirtualServerBuilder::AddError(const std::string& error_code, const std::string& error_page)
{
	m_LocationBuilder.AddError(error_code, error_page);
}

void VirtualServerBuilder::AddCgi(const std::string& cgi_path)
{
	m_LocationBuilder.AddCgi(cgi_path);
}

void VirtualServerBuilder::AddUpload(const std::string& upload_path)
{
	m_LocationBuilder.AddUpload(upload_path);
}

void VirtualServerBuilder::AddUriProperty(const std::string& uri, const std::string& prop_name, const std::string& prop_value)
{
	m_LocationBuilder.AddUriProperty(uri, prop_name, prop_value);
}

void VirtualServerBuilder::BuildAllRoutes()
{
	m_LocationBuilder.BuildAllRoutes();
	m_VS->m_StandardRoutes = m_LocationBuilder.GetStandardRoutes();
	m_VS->m_ErrorRoutes = m_LocationBuilder.GetErrors();

	std::map<std::string, std::map<std::string, std::string> > uri_props = m_LocationBuilder.GetUriProperties();
	for (std::map<std::string, std::map<std::string, std::string> >::iterator it = uri_props.begin(); it != uri_props.end(); ++it)
	{
		const std::string& uri = it->first;
		std::map<std::string, std::string>& props = it->second;
		VirtualServer::UriProps& property = m_VS->m_UriToProperties[uri];
		property.path = props.at(LocationNames::UriPath);
		property.uri = uri;

		if (props.count(LocationNames::ExceptedMethods))
		{
			std::string& excepted_methods = props.at(LocationNames::ExceptedMethods);
			while (!excepted_methods.empty())
			{
				size_t delim = excepted_methods.find(',');
				property.excepted_methods.push_back(excepted_methods.substr(0, delim));
				if (delim == std::string::npos)
				{
					excepted_methods.clear();
				}
				else
				{
					excepted_methods.erase(0, delim + 1);
				}
			}
		}
	}
}

