#include "SocketContainer.h"
#include "Cluster.h"

#include <tuple>

void SocketContainer::AddSocket(IOSocket socket, const short *events, size_t events_size)
{
	std::pair<socket_wrapper_type::iterator, bool> ins_result = m_SocketWrappers.insert(std::make_pair(socket.GetFd(), socket));
	AddToPdfs(ins_result.first->first, events, events_size);
}

size_t SocketContainer::AddToPdfs(int socket, const short *events, size_t events_size)
{
	pollfd inserting = {};
	inserting.fd = socket;

	for (size_t i = 0; i < events_size; ++i)
	{
		inserting.events |= events[i];
	}

	m_SocketsPdfs.push_back(inserting);
	return m_SocketsPdfs.size() - 1;
}

void SocketContainer::DelSocket(const IOSocket *deleted_socket, size_t socket_pdfs_idx)
{
	DelFromPdfs(socket_pdfs_idx);
	m_SocketWrappers.erase(deleted_socket->GetFd());
}

void SocketContainer::DelFromPdfs(size_t deleted_idx)
{
	m_SocketsPdfs.erase(m_SocketsPdfs.begin() + deleted_idx);
}

pollfd *SocketContainer::GetPdfs()
{
	return m_SocketsPdfs.data();
}

size_t SocketContainer::GetPdfsSize() const
{
	return m_SocketsPdfs.size();
}

SocketEvent SocketContainer::GetEvent(size_t idx) const
{
	SocketEvent event = {};
	if (m_SocketsPdfs[idx].revents == 0)
	{
		event.socket = NULL;
	}
	else
	{
		event.socket = &m_SocketWrappers.at(m_SocketsPdfs[idx].fd);
		if (m_SocketsPdfs[idx].revents & POLLIN)
		{
			if (event.socket->IsListening())
			{
//				event.handlers.push_back(&Cluster::Accept);
				event.handler = &Cluster::Accept;
			}
			else
			{
//				event.handlers.push_back(&Cluster::Receive);
				event.handler = &Cluster::Receive;
			}
		}
		if (m_SocketsPdfs[idx].revents & POLLOUT)
		{
//			event.handlers.push_back(&Cluster::Send);
			event.handler = &Cluster::Send;
		}
	}
	return event;
}
