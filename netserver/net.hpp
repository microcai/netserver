/*
 *
 * Copyright (C) 2009 jack.wgm, microcai.
 * For conditions of distribution and use, see copyright notice 
 * in (http://code.google.com/p/netsever/source/browse/trunk/COPYING)
 *
 * Author: jack.wgm
 * Email:  jack.wgm@gmail.com
 */

#ifndef NET_H__
#define NET_H__

#if _MSC_VER
#pragma once
#endif

#include "jobqueue.hpp"

using boost::asio::ip::tcp;

class server;
class message;
class session;
class io_service_pool;

typedef boost::shared_ptr<session> session_ptr;
typedef boost::shared_ptr<message> message_ptr;

#ifdef SOCKET_SSL
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
#endif // SOCKET_SSL

//////////////////////////////////////////////////////////////////////////
class message
{
public:
	enum { header_length = 8 };
	enum { max_body_length = 102400 };

	message() 
		: body_length_(0)
		, msg_(NULL)
	{}

	message(const message& msg)
	{
		memcpy(data_, msg.data_, message::header_length + message::max_body_length);
		body_length_ = msg.body_length_;
		msg_ = (packMsgPtr)data_;
		session_ = msg.session_;
	}

	const char* data() const
	{
		return data_;
	}

	char* data()
	{
		return data_;
	}

	size_t length() const
	{
		return header_length + body_length_;
	}

	void setsession(const session_ptr& _session)
	{
		session_ = _session;
	}

	void getsession(session_ptr& _session)
	{
		_session = session_.lock();
	}

	const char* body() const
	{
		return data_ + header_length;
	}

	char* body()
	{
		return data_ + header_length;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void body_length(size_t length)
	{
		body_length_ = length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool decode_header()
	{
		msg_ = (packMsgPtr)data_;
		if (msg_->MsgHead.packsize > header_length + max_body_length ||
			msg_->MsgHead.packsize < header_length) 
		{
			body_length_ = 0;
			return false;
		}
		body_length_ = msg_->MsgHead.packsize - header_length;
		return true;
	}

	bool check_body(size_t bytes_transferred)
	{
		msg_ = (packMsgPtr)data_;
		if (msg_->MsgHead.packsize > header_length + max_body_length ||
			msg_->MsgHead.packsize < header_length ||
			bytes_transferred != msg_->MsgHead.packsize - header_length) 
		{
			body_length_ = 0;
			return false;
		}
		body_length_ = msg_->MsgHead.packsize - header_length;
		return true;
	}

	void encode_header()
	{
		msg_ = (packMsgPtr)data_;
		msg_->MsgHead.packsize = body_length_ + header_length;
	}

	message& operator =(message &msg)
	{
		memcpy(data_, msg.data_, header_length + max_body_length);
		body_length_ = msg.body_length_;		
		msg_ = (packMsgPtr)data_;
		session_ = msg.session_;
		return (*this);
	}

	const packMsgPtr msg()
	{
		return msg_;
	}

private:
	char data_[header_length + max_body_length];
	size_t body_length_;
	boost::weak_ptr<session> session_;
	packMsgPtr msg_;
};

//////////////////////////////////////////////////////////////////////////
class io_service_pool
	: private boost::noncopyable
{
public:
	explicit io_service_pool(std::size_t pool_size);

	void run();
	void stop();

	boost::asio::io_service& get_io_service();

private:
	typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
	typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

	std::vector<io_service_ptr> io_services_;
	std::vector<work_ptr> work_;
	std::size_t next_io_service_;
};

//////////////////////////////////////////////////////////////////////////
#if defined(SOCKET_SSL) // 使用ssl加密.

class session
	: public boost::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service& io_service, 
			jobqueue<message>& jobwork, 
			boost::object_pool<message>& message_pool, 
			boost::asio::ssl::context& context);
	~session();

	ssl_socket::lowest_layer_type& socket();

	void handle_handshake(const boost::system::error_code& error);

	void start();
	void write();

	void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_read_body(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);

private:
	ssl_socket socket_;
	jobqueue<message>& jobwork_;
	// message message_;
	message* message_;

	boost::asio::io_service::strand strand_;
	boost::object_pool<message>& message_pool_;
};

//////////////////////////////////////////////////////////////////////////
class server
{
public:
	server(short port, jobqueue<message>& jobwork, std::size_t io_service_pool_size = 4);

	void run();
	void stop();
	void handle_accept(session_ptr new_session,
		const boost::system::error_code& error);

	std::string get_password() const;

private:
	jobqueue<message>& jobwork_;
	io_service_pool io_service_pool_;
	tcp::acceptor acceptor_;

	boost::asio::ssl::context context_;
	boost::object_pool<message> message_pool_;
};

#else // 未使用ssl加密.

class session
	: public boost::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service& io_service, jobqueue<message>& jobwork, boost::object_pool<message>& message_pool);
	~session();
	tcp::socket& socket();

	void start();
	void write();

	void handle_read_head(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_read_body(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);

private:
	tcp::socket socket_;
	jobqueue<message>& jobwork_;
	// message message_;
	message* message_;

	boost::asio::io_service::strand strand_;
	boost::object_pool<message>& message_pool_;
};

//////////////////////////////////////////////////////////////////////////
class server
{
public:
	server(short port, jobqueue<message>& jobwork, std::size_t io_service_pool_size = 4);

	void run();
	void stop();
	void handle_accept(session_ptr new_session,
		const boost::system::error_code& error);

private:
	jobqueue<message>& jobwork_;
	io_service_pool io_service_pool_;
	tcp::acceptor acceptor_;

	boost::object_pool<message> message_pool_;
};

#endif // SOCKET_SSL



#endif // NET_H__