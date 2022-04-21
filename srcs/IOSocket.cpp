#include "IOSocket.h"

IOSocket::IOSocket(int socket_fd, bool is_listening, const PhysicalServer *server)
	: m_SocketFd(socket_fd)
	, m_IsListening(is_listening)
	, m_Server(server)
{}

int IOSocket::GetFd() const
{
	return m_SocketFd;
}

bool IOSocket::IsListening() const
{
	return m_IsListening;
}

const PhysicalServer *IOSocket::GetServer() const
{
	return m_Server;
}
