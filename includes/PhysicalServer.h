#pragma once

#include <deque>
#include <queue>

#include "VirtualServer.h"
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
			bool header_filled = false;
			bool is_finished = false;
			bool is_chunked = false;
		};

		std::queue<ReceivingMsg> m_Queue;

	public:
		typedef ReceivingMsg receiving_msg_type;

	public:
		ReceivingMsg& GetLastNotFilled()
		{
			if (m_Queue.empty() || (!m_Queue.empty() && m_Queue.back().is_finished))
			{
				m_Queue.push(ReceivingMsg());
			}
			return m_Queue.back();
		}
	};

	ReceivingQueue m_PendingRequests;

public:
	std::deque<raii_ptr<VirtualServer> > m_VirtualServers;

	PhysicalServer(const std::deque<VirtualServer*>& servers);

	void ReadHeaders(std::string& recv_buffer);
/*
	void ReadFullMessage(std::string& recv_buffer);
*/
};
