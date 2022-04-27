#include "VirtualServer.h"

VirtualServer::VirtualServer(const std::string& mServerName,
			  size_t mBodyLimit,
			  const std::string& mErrorPath,
			  const std::map<std::string, std::string>& mRoutes)
	: m_ServerName(mServerName)
	, m_BodyLimit(mBodyLimit)
	, m_ErrorPath(mErrorPath)
	, m_routes(mRoutes)
{}
