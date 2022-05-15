#include "Cluster.h"

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

#define POLL_TIMEOUT -1

const short Cluster::ReadEvent[] = {POLLIN};
const short Cluster::WriteEvent[] = {POLLOUT};
const short Cluster::ReadWriteEvent[] = {POLLIN, POLLOUT};

const size_t Cluster::ReadOrWriteSize = sizeof(Cluster::ReadEvent) / sizeof(short);
const size_t Cluster::ReadAndWriteSize = sizeof(Cluster::ReadWriteEvent) / sizeof(short);

Cluster::Cluster(const IConfig& conf)
	: m_Config(conf)
{
	Init();
}

void Cluster::Run()
{
    while(true)
	{
        int poll_cnt = poll(m_Sockets.GetPdfs(), m_Sockets.GetPdfsSize(), POLL_TIMEOUT);

        if (poll_cnt == -1)
        {
            std::cerr << "poll: " << strerror(errno) << std::endl;;
            exit(1);
        }

        for (size_t i = 0, visited = 0; i < m_Sockets.GetPdfsSize(); ++i)
        {
			SocketEvent event = m_Sockets.GetEvent(i);
			if (!event.socket)
			{
				continue;
			}
//			for (size_t handler_idx = 0; handler_idx < event.handlers.size(); ++handler_idx)
//			{
//				(this->*(event.handlers[handler_idx]))(event.socket, i);
//			}
			(this->*(event.handler))(event.socket, i);
            ++visited;
            if (static_cast<int>(visited) == poll_cnt)
            {
                break;
            }
        }
    }
}

void Cluster::Init()
{
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC; /// auto determining IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; /// use my IP address as host. Write INADDR_ANY to IP address

	const IConfig::physical_ports_ip_cont& physical_endpoints = m_Config.GetPhysicalEndpoints();
	for (IConfig::physical_ports_ip_cont::const_iterator it = physical_endpoints.begin(); it != physical_endpoints.end(); ++it)
	{
		CreatePhysicalServer(&hints, it->second, it->first);
	}
}

void Cluster::CreatePhysicalServer(addrinfo *hints, const std::string& ip_number, const std::string& port_number)
{
	addrinfo *results;
	int rv;
	if ((rv = getaddrinfo((ip_number.empty()) ? NULL : ip_number.c_str(), port_number.c_str(), hints, &results)) != 0)
	{
		std::cerr << "Getaddrinfo error: " << gai_strerror(rv) << std::endl;
		exit(1);
	}

	addrinfo *curr;
	int listening_socket;
	for (curr = results; curr != NULL; curr = curr->ai_next)
	{
		if ((listening_socket = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
		{
			std::cerr << "socket: " << strerror(errno) << std::endl;;
			continue;
		}

		int yes = 1; /// may be need char for Sun and Win
		if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) /// skip "Address already in use" when old socket wasn't closed
		{
			std::cerr << "setsockopt: " << strerror(errno) << std::endl;;
			exit(1);
		}

		if (bind(listening_socket, curr->ai_addr, curr->ai_addrlen) == -1)
		{
			std::cerr << "bind: " << strerror(errno) << std::endl;;
			continue;
		}
		break;
	}

	if (curr == NULL)
	{
		std::cerr << "Error getting listening socket" << std::endl;
		exit(1);
	}

	std::cout
			<< "Host ip: "
			<< GetPrintableIP(curr->ai_addr);

	freeaddrinfo(results);

	if (listen(listening_socket, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "listen: " << strerror(errno) << std::endl;;
		exit(1);
	}

	std::cout
		<< ", listening socket: "
		<< listening_socket
		<< std::endl;

	m_Servers.push_back(raii_ptr<PhysicalServer>(new PhysicalServer(m_Config.GetServersByPort(port_number))));
	m_Sockets.AddSocket(IOSocket(listening_socket, true, m_Servers.back().get()), Cluster::ReadEvent, ReadOrWriteSize);
}

std::string Cluster::GetPrintableIP(sockaddr *addr_info) const
{
    static char ip[INET6_ADDRSTRLEN];
    inet_ntop(addr_info->sa_family, GetInputAddr(addr_info), ip, sizeof(ip));
	return ip;
}

void *Cluster::GetInputAddr(sockaddr *sa) const
{
    if (sa->sa_family == AF_INET)
    {
        return &(reinterpret_cast<sockaddr_in*>(sa)->sin_addr);
    }
    return &(reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr);
}

void Cluster::Accept(const IOSocket *event_socket, size_t /*socket_pdfs_idx*/) /// Почему от одного клиента приходит 2 соединения (браузер)
{
	sockaddr_storage connecting_address = {};
	socklen_t storage_size = sizeof(connecting_address);
	int connected_socket = accept(event_socket->GetFd(), reinterpret_cast<sockaddr*>(&connecting_address), &storage_size);
	if (connected_socket == -1)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;;
	}
	else
	{
		m_Sockets.AddSocket(IOSocket(connected_socket, false, event_socket->GetServer()), Cluster::ReadWriteEvent, Cluster::ReadAndWriteSize);

		std::cout
				<< "New connection from "
				<< GetPrintableIP(reinterpret_cast<sockaddr*>(&connecting_address))
				<< " with socket "
				<< connected_socket
				<< " remote address family: "
				<< (connecting_address.ss_family == AF_INET ? "IPv4" : "IPv6")
				<< std::endl;
	}
}

void Cluster::Receive(const IOSocket *event_socket, size_t socket_pdfs_idx)
{
	/**
	 * \todo
	 * чтение в цикле всего сообщения и прокидка его в PhysicalServer на парсинг и роутинг
	 */
	static const int MaxIp6Datagram = 65575;
	char rec_buf[MaxIp6Datagram + 1];

	int bytes_cnt = recv(event_socket->GetFd(), rec_buf, sizeof(rec_buf), 0);
	rec_buf[bytes_cnt] = '\0';

	if (bytes_cnt <= 0)
	{
		if (bytes_cnt == 0)
		{
			std::cout << "Socket " << event_socket->GetFd() << " closed connection" << std::endl;
		}
		else if (bytes_cnt == -1)
		{
			std::cerr << "recv: " << strerror(errno) << std::endl;; /// maybe forbidden due subject /// появлялось recv: Connection reset by peer. Видимо пытался писать в закрытый сокет (надо протестить)
		}
		close(event_socket->GetFd());
		m_Sockets.DelSocket(event_socket, socket_pdfs_idx);
	}
	else
	{
		std::string str_buf(rec_buf);
		event_socket->GetServer()->ReadHeaders(str_buf);
		/// Parse Request
		/// Transfer to VirtualServer
	}
}

void Cluster::Send(const IOSocket */*event_socket*/, size_t /*socket_pdfs_idx*/)
{
	/**
	 * \todo
	 * Проверка готовности ответа для клиента и отправка
	 */
//	if (send(event_socket->GetFd(), "Hello", 5, 0) == -1)
//	{
//		std::cerr << "send: " << strerror(errno) << std::endl;; /// maybe forbidden due subject
//	}
}
