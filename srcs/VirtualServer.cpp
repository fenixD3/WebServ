#include "VirtualServer.h"

const std::string LocationNames::Index = "index";
const std::string LocationNames::Cgi = "cgi";
const std::string LocationNames::Upload = "upload";
const std::string LocationNames::UriPath = "path";
const std::string LocationNames::ExceptedMethods = "limit_except";

VirtualServer::VirtualServer() {}

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
	for (std::map<std::string, std::string>::iterator it = m_StandardRoutes.begin(); it != m_StandardRoutes.end(); ++it)
	{
		it->second = m_RootPath + it->second;
	}
	for (std::map<std::string, std::string>::iterator it = m_Errors.begin(); it != m_Errors.end(); ++it)
	{
		it->second = m_RootPath + it->second;
	}
	for (std::map<std::string, std::map<std::string, std::string> >::iterator it = m_UriToProperties.begin(); it != m_UriToProperties.end(); ++it)
	{
		it->second[LocationNames::UriPath] = m_RootPath + it->second[LocationNames::UriPath] + it->first;
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
	: m_VS(make_default_ptr<VirtualServer>())
{}

VirtualServerBuilder::operator VirtualServer*()
{
	VirtualServer* result = m_VS.get();
	m_VS = NULL;
	return result;
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
