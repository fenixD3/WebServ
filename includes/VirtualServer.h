#pragma once

#include <vector>
#include <string>
#include <map>

/*
class VirtualServerBuilder;
*/

class VirtualServer
{
private:
    std::string m_ServerName;
	size_t m_BodyLimit;
	std::string m_ErrorPath;
	std::map<std::string, std::string> m_routes;

private:
/*    friend class VirtualServerBuilder;*/

public:
	VirtualServer(const std::string& mServerName,
				  size_t mBodyLimit,
				  const std::string& mErrorPath,
				  const std::map<std::string, std::string>& mRoutes);

	void PrintServerName() const;
};

/*class VirtualServerBuilder
{
private:
    VirtualServerBuilder(const IConfig& config)
        : m_Config(config)
    {}

private:
    VirtualServer m_VS;
	const IConfig& m_Config;

public:
    operator VirtualServer()
    {
        return m_VS;
    }

    static VirtualServerBuilder Build(const IConfig& config)
    {
		static VirtualServerBuilder builder(config);
        return builder;
    }

    VirtualServerBuilder& AddServerPort()
    {
//        m_VS.m_Port = m_Config.GetPhysicalPorts();
        return *this;
    }

    VirtualServerBuilder& AddHostIP()
    {
//        m_VS.m_HostIP = m_Config.GetHostIP();
        return *this;
    }

    VirtualServerBuilder& AddServerName()
    {
//        m_VS.m_ServerName = m_Config.GetServerName();
        return *this;
    }
};*/
