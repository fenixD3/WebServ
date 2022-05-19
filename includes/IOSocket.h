#pragma once

class PhysicalServer;

class IOSocket
{
private:
	int m_SocketFd;
	bool m_IsListening;
	PhysicalServer *m_Server;
	bool m_ClientSideIsClosed;

public:
	IOSocket(int socket_fd, bool is_listening, PhysicalServer *server);

	int GetFd() const;
	bool IsListening() const;
	PhysicalServer *GetServer() const;
	bool IsClientSideClosed() const;
	void SetClientSideClosing(bool closing);
};
