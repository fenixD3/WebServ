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
	ReceivingQueue::receiving_msg_type& filling_msg = m_PendingRequests.GetLastNotFilledHeader();
	filling_msg.header_filled = FillRequestMsg(filling_msg, recv_buffer, HEADER);
	if (filling_msg.header_filled)
	{
		HttpRequestBuilder::http_request uncooked_req =
				HttpRequestBuilder::GetInstance().BuildHttpRequestHeader(filling_msg.msg);
		filling_msg.http_request = &uncooked_req;
		filling_msg.body_size_for_read = uncooked_req.GetContentLength();
	}
}

void IOSocket::ReadBody(std::string& recv_buffer)
{
	ReceivingQueue::receiving_msg_type& filling_msg = m_PendingRequests.GetLastNotFilled();
	HttpRequest *http_req = filling_msg.http_request.get();
	if (!http_req || http_req->IsValid())
	{
		std::cerr << "Bad request" << std::endl;
		return;
	}
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
			has_been_filled = true;
			end_pos += pattern.size();
		}
		filling_msg.msg += recv_buffer.substr(0, end_pos);
		recv_buffer.erase(0, end_pos);
	}
	else
	{
		size_t how_to_reading = std::min(recv_buffer.size(), filling_msg.body_size_for_read);

		filling_msg.msg += recv_buffer.substr(0, how_to_reading);
		recv_buffer.erase(0, how_to_reading);
		filling_msg.body_size_for_read -= how_to_reading;

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

IOSocket::sending_msg_const_ptr IOSocket::GetNextSendable()
{
	for (IOSocket::ReceivingQueue::queue_type::iterator it = m_PendingRequests.Begin(); it != m_PendingRequests.End(); )
	{
		if (!it->is_finished)
		{
			return NULL;
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
