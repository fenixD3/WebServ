#include "IOSocket.h"

IOSocket::IOSocket(int socket_fd, bool is_listening, PhysicalServer *server)
	: m_SocketFd(socket_fd)
	, m_IsListening(is_listening)
	, m_Server(server)
	, m_ClientSideIsClosed(false)
{}

int IOSocket::GetFd() const
{
	return m_SocketFd;
}

bool IOSocket::IsListening() const
{
	return m_IsListening;
}

PhysicalServer *IOSocket::GetServer() const
{
	return m_Server;
}

bool IOSocket::IsClientSideClosed() const
{
	return m_ClientSideIsClosed;
}

void IOSocket::SetClientSideClosing(bool closing)
{
	m_ClientSideIsClosed = closing;
}


