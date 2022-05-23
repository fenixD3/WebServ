#pragma once

#include <deque>
#include <queue>
#include <string>

#include "raii_ptr.h"

class PhysicalServer;
class HttpRequest;

class IOSocket
{
private:
	class ReceivingQueue
	{
		struct ReceivingMsg;
	public:
		typedef std::deque<ReceivingMsg> queue_type;

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

		queue_type m_Queue;

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

		queue_type::iterator Begin()
		{
			return m_Queue.begin();
		}

		queue_type::iterator End()
		{
			return m_Queue.end();
		}

		queue_type::iterator Erase(queue_type::iterator position)
		{
			return m_Queue.erase(position);
		}
	};


	enum ReadingTypePattern
	{
		HEADER,
		CHUNKED_BODY,
		SIMPLE_BODY
	};

	class SendingQueue
	{
	private:
		struct SendingMsg
		{
			friend class SendingQueue;
		private:
			std::string msg;
			size_t sending_size;

		public:
			SendingMsg(const std::string& response_msg);

			const char *GetSendableFormat() const
			{
				return msg.c_str();
			}
		};

		std::queue<SendingMsg> m_Queue;

	public:
		typedef SendingMsg sending_msg_type;

	public:
		const sending_msg_type *GetNext() const
		{
			if (m_Queue.empty())
			{
				return NULL;
			}
			return &m_Queue.front();
		}

		void Update(size_t sent_bytes)
		{
			SendingMsg& current_msg = m_Queue.front();

			if (sent_bytes == current_msg.sending_size)
			{
				m_Queue.pop();
			}
			else
			{
				current_msg.sending_size -= sent_bytes;
				current_msg.msg.erase(0, sent_bytes);
			}
		}

		void PushNew(const std::string& msg)
		{
			m_Queue.push(SendingMsg(msg));
		}
	};

	ReceivingQueue m_PendingRequests;
	SendingQueue m_PendingResponses;

private:
	int m_SocketFd;
	bool m_IsListening;
	PhysicalServer *m_Server;
	bool m_ClientSideIsClosed;

public:
	typedef const SendingQueue::sending_msg_type* sending_msg_const_ptr;

public:
	IOSocket(int socket_fd, bool is_listening, PhysicalServer *server);

	int GetFd() const;
	bool IsListening() const;
	PhysicalServer *GetServer() const;
	bool IsClientSideClosed() const;
	void SetClientSideClosing(bool closing);

	void ReadHeaders(std::string& recv_buffer);
	void ReadBody(std::string& recv_buffer);

	sending_msg_const_ptr GetNextSendable();
	void UpdateSendingQueue(ssize_t sent_bytes);

private:
	bool FillRequestMsg(ReceivingQueue::receiving_msg_type& filling_req,
						std::string& recv_buffer,
						ReadingTypePattern reading_pattern);

	const std::string& TypePatternToString(ReadingTypePattern type) const;
};
