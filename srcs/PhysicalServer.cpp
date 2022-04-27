#include "PhysicalServer.h"

PhysicalServer::PhysicalServer(const std::deque<VirtualServer*>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		m_VirtualServers.push_back(raii_ptr<VirtualServer>(servers[i]));
	}
}

void PhysicalServer::ReadHeaders(std::string& recv_buffer)
{
	const std::string EndPattern = "\r\n\r\n";

	while (!recv_buffer.empty())
	{
		size_t end_pos = recv_buffer.find(EndPattern);
		ReceivingQueue::receiving_msg_type& current_msg = m_PendingRequests.GetLastNotFilled();

		if (!current_msg.header_filled)
		{
			current_msg.msg += recv_buffer.substr(0, end_pos + EndPattern.size());
			recv_buffer.erase(0, end_pos + EndPattern.size());
			if (end_pos != std::string::npos)
			{
				current_msg.header_filled = true;
			}
		}
	}
}

/*void PhysicalServer::ReadFullMessage(std::string& recv_buffer)
{
	const std::string ContentLength = "Content-Length: ";
	const std::string TransferChunked = "Transfer-Encoding: chunked";
	const std::string EndPattern = "\r\n\r\n";
	const std::string EndChunkPattern = "0\r\n\r\n";

	while (!recv_buffer.empty())
	{
		size_t end_pos = recv_buffer.find(EndPattern);
		ReceivingQueue::receiving_msg_type& current_msg = m_PendingRequests.GetLastNotFilled();

		if (end_pos != std::string::npos)
		{
			if (!current_msg.header_filled)
			{
				current_msg.header_filled = true;

				size_t content_length_pos = recv_buffer.find(ContentLength);
				if (content_length_pos == std::string::npos)
				{
					current_msg.is_chunked = true;

					size_t end_body_chunked_pos = recv_buffer.find(EndChunkPattern);
					if (end_body_chunked_pos != std::string::npos)
					{
						current_msg.is_finished = true;
						current_msg.msg += recv_buffer.substr(0, end_body_chunked_pos + EndChunkPattern.size());
						recv_buffer.erase(0, end_body_chunked_pos + EndChunkPattern.size());
						continue;
					}
					size_t chunk_end_pos = recv_buffer.rfind(EndPattern); /// если в пакете несколько чанков
					current_msg.msg += recv_buffer.substr(0, chunk_end_pos + EndPattern.size());
					recv_buffer.erase(0, chunk_end_pos + EndPattern.size());
				}
				else
				{
					size_t body_end_pos = recv_buffer.find(EndPattern, end_pos + EndPattern.size());
					current_msg.msg += recv_buffer.substr(0, body_end_pos + EndPattern.size());
					recv_buffer.erase(0, body_end_pos + EndPattern.size());
					if (body_end_pos != std::string::npos)
					{
						current_msg.is_finished = true;
						continue;
					}
				}
			}
		}
		else
		{
			current_msg.msg += recv_buffer.substr(0, end_pos + EndPattern.size());
			recv_buffer.erase(0, end_pos + EndPattern.size());
		}
	}
}*/
