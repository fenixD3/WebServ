#include "IOSocket.h"
#include "PhysicalServer.h"
#include "RequestBuilder.h"

IOSocket::ReceivingQueue::ReceivingMsg::ReceivingMsg()
	: header_filled(false)
	, is_finished(false)
	, http_request(make_default_ptr<HttpRequest>())
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
	ReceivingQueue::receiving_msg_type& filling_msg = m_PendingRequests.GetLastNotFilled();
	if (filling_msg.header_filled)
	{
		return;
	}

	filling_msg.header_filled = FillRequestMsg(filling_msg, recv_buffer, HEADER);
	if (filling_msg.header_filled)
	{
		std::pair<bool, std::string> boundary_found =
			HttpRequestBuilder::GetInstance().BuildHttpRequestHeader(filling_msg.msg, *filling_msg.http_request);
		if (boundary_found.first)
		{
			filling_msg.header_filled = false;
			filling_msg.boundary_end = boundary_found.second;
			if (!recv_buffer.empty())
			{
				ReadHeaders(recv_buffer);
			}
		}
		filling_msg.body_size_for_read = filling_msg.http_request->GetContentLength();
	}
}

void IOSocket::ReadBody(std::string& recv_buffer)
{
	ReceivingQueue::receiving_msg_type& filling_msg = m_PendingRequests.GetLastNotFilled();
	if (!filling_msg.header_filled)
	{
		return;
	}

	HttpRequest *http_req = filling_msg.http_request.get();
	TransferEncoding encoding_type = http_req->GetTransferEncoding();

	if (encoding_type == CHUNKED)
	{
		filling_msg.is_finished = FillRequestMsg(filling_msg, recv_buffer, CHUNKED_BODY);
	}
	else
	{
		filling_msg.is_finished = FillRequestMsg(filling_msg, recv_buffer, SIMPLE_BODY);
	}

	if (filling_msg.is_finished)
	{
		HttpRequestBuilder::GetInstance().BuildHttpRequestBody(*http_req, filling_msg.msg);
	}
}

bool IOSocket::FillRequestMsg(ReceivingQueue::receiving_msg_type& filling_msg,
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
			end_pos += pattern.size();
		}
		filling_msg.msg += recv_buffer.substr(0, end_pos);
		recv_buffer.erase(0, end_pos);

		if (filling_msg.msg.find(pattern, filling_msg.msg.size() - pattern.size() - 1) != std::string::npos)
		{
			has_been_filled = true;
		}
	}
	else if (!filling_msg.boundary_end.empty())
	{
		size_t boundary_body_pos = recv_buffer.find(filling_msg.boundary_end);

		if (boundary_body_pos != std::string::npos)
		{
			boundary_body_pos += filling_msg.boundary_end.size();
		}
		filling_msg.msg += recv_buffer.substr(0, boundary_body_pos);

		/// TODO: Write Event (temp)
		filling_msg.body_size_for_read -= recv_buffer.substr(0, boundary_body_pos).size();
//		std::cerr << "Still for read: " << filling_msg.body_size_for_read << std::endl;
		/// TODO: Write Event (temp_end)

		recv_buffer.erase(0, boundary_body_pos);

		if (filling_msg.msg.find(filling_msg.boundary_end) != std::string::npos)
		{
			has_been_filled = true;
		}
	}
	else
	{
		size_t how_to_reading = std::min(recv_buffer.size(), filling_msg.body_size_for_read);

		filling_msg.msg += recv_buffer.substr(0, how_to_reading);
		recv_buffer.erase(0, how_to_reading);
		filling_msg.body_size_for_read -= how_to_reading;

//		std::cerr << "Still for read: " << filling_msg.body_size_for_read << std::endl;

		if (filling_msg.body_size_for_read == 0)
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

bool IOSocket::PrepareNextSendable()
{
	for (IOSocket::ReceivingQueue::queue_type::iterator it = m_PendingRequests.Begin(); it != m_PendingRequests.End(); )
	{
		if (!it->is_finished)
		{
			return false;
		}
		std::string response = m_Server->ProcessRequest(it->http_request.get());
		m_PendingResponses.PushNew(response);
		it = m_PendingRequests.Erase(it);
	}
	return true;
}

IOSocket::sending_msg_const_ptr IOSocket::GetNextResponse() const
{
	return m_PendingResponses.GetNext();
}

void IOSocket::UpdateSendingQueue(ssize_t sent_bytes)
{
	m_PendingResponses.Update(sent_bytes);
}
