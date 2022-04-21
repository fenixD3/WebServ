#pragma once

class PhysicalServer;

class IOSocket
{
private:
	int m_SocketFd;
	bool m_IsListening;
	const PhysicalServer *m_Server;

public:
	IOSocket(int socket_fd, bool is_listening, const PhysicalServer *server);

	int GetFd() const;
	bool IsListening() const;
	const PhysicalServer *GetServer() const;
};
