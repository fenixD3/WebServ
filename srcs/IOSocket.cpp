#include "IOSocket.h"
#include "PhysicalServer.h"
#include "RequestBuilder.h"

IOSocket::ReceivingQueue::ReceivingMsg::ReceivingMsg()
	: header_filled(false)
	, is_finished(false)
{}

IOSocket::SendingQueue::SendingMsg::SendingMsg(const std::string& response_msg)
	: msg(response_msg)
	, sending_size(response_msg.size())
{}

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

void IOSocket::ReadHeaders(std::string& recv_buffer)
{
	while (!recv_buffer.empty())
	{
		ReceivingQueue::receiving_msg_type& filling_req = m_PendingRequests.GetLastNotFilledHeader();
		bool has_been_filled = FillRequestMsg(filling_req, recv_buffer, HEADER);
		if (has_been_filled)
		{
			filling_req.header_filled = true;
		}
	}
	for (size_t i = 0; i < m_PendingRequests.Size() || m_PendingRequests[i].header_filled; ++i)
	{
		if (!m_PendingRequests[i].http_request)
		{
			HttpRequestBuilder::http_request uncooked_req =
					HttpRequestBuilder::GetInstance().BuildHttpRequestHeader(m_PendingRequests[i].msg);
			m_PendingRequests[i].http_request = &uncooked_req;
			m_PendingRequests[i].body_size_for_read = uncooked_req.GetContentLength();
		}
	}
}

void IOSocket::ReadBody(std::string& recv_buffer)
{
	for (size_t i = 0; !recv_buffer.empty() || i < m_PendingRequests.Size(); ++i)
	{
		if (!m_PendingRequests[i].header_filled || m_PendingRequests[i].is_finished)
		{
			continue;
		}

		HttpRequest *http_req = m_PendingRequests[i].http_request.get();
		TransferEncoding encoding_type = http_req->GetTransferEncoding();
		bool has_been_filled;
		if (encoding_type == CHUNKED)
		{
			has_been_filled = FillRequestMsg(m_PendingRequests[i], recv_buffer, CHUNKED_BODY);
		}
		else
		{
			has_been_filled = FillRequestMsg(m_PendingRequests[i], recv_buffer, SIMPLE_BODY);
		}

		if (has_been_filled)
		{
			m_PendingRequests[i].is_finished = true;
			HttpRequestBuilder::GetInstance().BuildHttpRequestBody(*m_PendingRequests[i].http_request, m_PendingRequests[i].msg);
		}
	}
}

bool IOSocket::FillRequestMsg(ReceivingQueue::receiving_msg_type& filling_req,
									std::string& recv_buffer,
									ReadingTypePattern reading_pattern)
{
	const std::string& pattern = TypePatternToString(reading_pattern);
	bool has_been_filled = false;

	if (!pattern.empty())
	{
		size_t end_pos = recv_buffer.find(pattern);

		if (end_pos != std::string::npos)
		{
			has_been_filled = true;
			end_pos += pattern.size();
		}
		filling_req.msg += recv_buffer.substr(0, end_pos);
		recv_buffer.erase(0, end_pos);
	}
	else
	{
		size_t how_to_reading = std::min(recv_buffer.size(), filling_req.body_size_for_read);

		filling_req.msg += recv_buffer.substr(0, how_to_reading);
		recv_buffer.erase(0, how_to_reading);
		filling_req.body_size_for_read -= how_to_reading;

		if (filling_req.body_size_for_read == 0)
		{
			has_been_filled = true;
		}
	}
	return has_been_filled;
}

const std::string& IOSocket::TypePatternToString(ReadingTypePattern type) const
{
	static const std::string HeaderEndPattern = "\r\n\r\n";
	static const std::string ChunkEndPattern = "0\r\n\r\n";
	static const std::string Empty;

	switch (type)
	{
		case HEADER:
			return HeaderEndPattern;
		case CHUNKED_BODY:
			return ChunkEndPattern;
		case SIMPLE_BODY:
		default:
			return Empty;
	}
}

IOSocket::sending_msg_const_ptr IOSocket::GetNextSendable()
{
	for (IOSocket::ReceivingQueue::queue_type::iterator it = m_PendingRequests.Begin();
		it != m_PendingRequests.End(); )
	{
		if (!it->is_finished)
		{
			break;
		}
		std::string response = m_Server->ProcessRequest(it->http_request.get());
		m_PendingResponses.PushNew(response);
		it = m_PendingRequests.Erase(it);
	}
	return m_PendingResponses.GetNext();
}

void IOSocket::UpdateSendingQueue(ssize_t sent_bytes)
{
	m_PendingResponses.Update(sent_bytes);
}
