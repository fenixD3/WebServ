#pragma once

#include <vector>

#include "ClientConnection.h"
#include "config/IConfig.h"

class VirtualServerBuilder;

class VirtualServer
{
private:
    std::string m_Port;
    std::string m_HostIP;
    std::string m_ServerName;
	std::map<std::string, std::string> m_Locations;
    std::vector<ClientConnection> m_Connections;

private:
    VirtualServer() {}

    friend class VirtualServerBuilder;
public:
    void AddConnection(const ClientConnection& client_connection)
    {
        m_Connections.push_back(client_connection);
    }
};

class VirtualServerBuilder
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
};
