#pragma once

#include <deque>

#include "RequestBuilder.h"
#include "VirtualServer.h"
#include "HttpRequest.h"
#include "raii_ptr.h"

class PhysicalServer
{
private:
	class ReceivingQueue
	{
	private:
		struct ReceivingMsg
		{
			std::string msg;
			bool header_filled;
			bool is_finished;
			raii_ptr<HttpRequest> http_request;
			size_t body_size_for_read;

			ReceivingMsg();
		};

		std::deque<ReceivingMsg> m_Queue;

	public:
		typedef ReceivingMsg receiving_msg_type;

	public:
		receiving_msg_type& GetLastNotFilledHeader()
		{
			if (m_Queue.empty() || (!m_Queue.empty() && m_Queue.back().header_filled))
			{
				m_Queue.push_back(ReceivingMsg());
			}
			return m_Queue.back();
		}

		size_t Size() const
		{
			return m_Queue.size();
		}

		receiving_msg_type& operator[](size_t ind)
		{
			return m_Queue[ind];
		}
	};

	enum ReadingTypePattern
	{
		HEADER,
		CHUNKED_BODY,
		SIMPLE_BODY
	};

	ReceivingQueue m_PendingRequests;

public:
	std::deque<raii_ptr<VirtualServer> > m_VirtualServers;

public:
	PhysicalServer(const std::deque<VirtualServer*>& servers);

	void ReadHeaders(std::string& recv_buffer);
	void ReadBody(std::string& recv_buffer);

private:
	bool FillRequestMsg(ReceivingQueue::receiving_msg_type& filling_req,
						std::string& recv_buffer,
						ReadingTypePattern reading_pattern);

	const std::string& TypePatternToString(ReadingTypePattern type) const;
};
