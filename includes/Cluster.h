#pragma once

#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <map>
#include <deque>

#include "PhysicalServer.h"
#include "SocketContainer.h"
#include "raii_ptr.h"
#include "config/IConfig.h"

#define LISTEN_BACKLOG 10

struct addrinfo;

class Cluster
{
	friend class SocketContainer;
private:
    static const short ReadEvent[1];
    static const short WriteEvent[1];
    static const short ReadWriteEvent[2];

	static const size_t ReadOrWriteSize;
	static const size_t ReadAndWriteSize;

	const IConfig& m_Config;

	std::deque<raii_ptr<PhysicalServer> > m_Servers;
	SocketContainer m_Sockets;

public:
    Cluster(const IConfig& conf);

    void Run();

private:
	void Accept(const IOSocket *event_socket, size_t socket_pdfs_idx);
	void Receive(const IOSocket *event_socket, size_t socket_pdfs_idx);
	void Send(const IOSocket *event_socket, size_t socket_pdfs_idx);

    void Init();
	void CreatePhysicalServer(addrinfo *hints, const std::string& port_number);

    void *GetInputAddr(sockaddr *sa) const;
    std::string GetPrintableIP(sockaddr *addr_info) const;
};