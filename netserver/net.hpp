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

/*/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////*/

class message
{
public:
	// 下面结构体使用1字节对齐.
	#pragma pack(push, 1)

	typedef struct _tagPacketHeader 
	{
		unsigned int type;							// 数据包类型.
		unsigned int checksum;						// 校检和,为其它三项之和. e.c type + rand + packsize.
		unsigned int rand;							// 随机数.
		unsigned int packsize;						// 数据包大小.
	} header, *headerPtr;

	// 恢复结构体字节对齐.
	#pragma pack(pop)

#define HEADER_LENGTH sizeof(message::header)	// 头大小.
	#define DEFAULT_BODY_LENGTH 10240			// 默认内存大小10k.

public:
	message() 
		: body_length_(0)
		, msg_(NULL)
	{
		// 默认分配内存块大小.
		data_ = new char[DEFAULT_BODY_LENGTH + HEADER_LENGTH];
		wptr_ = rptr_ = data_;
		data_size_ = DEFAULT_BODY_LENGTH + HEADER_LENGTH;
	}

	message(const message& msg)
	{		
		// 原内存块太小,重新分配内存.
		if (data_size_ < msg.data_size_) 
		{
			if (data_ != NULL)
				delete data_;
			data_size_ = msg.data_size_;
			data_ = new char[data_size_];
			wptr_ = rptr_ = data_;
		}
		
		// 拷贝内存.
		memcpy(data_, msg.data_, msg.data_size_);
		body_length_ = msg.body_length_;
		msg_ = (headerPtr)data_;
		session_ = msg.session_;
	}

	message& operator =(message &msg)
	{
		if (data_size_ < msg.data_size_) // 原内存块太小,重新分配内存.
		{
			if (data_ != NULL)
				delete data_;
			data_size_ = msg.data_size_;
			data_ = new char[msg.data_size_];
			wptr_ = rptr_ = data_;
		}

		// 拷贝内存.
		memcpy(data_, msg.data_, msg.data_size_);
		body_length_ = msg.body_length_;
		msg_ = (headerPtr)data_;
		session_ = msg.session_;

		return (*this);
	}

	~message()
	{
		if (data_)
			delete data_;
		wptr_ = rptr_ = data_ = NULL;
		msg_ = NULL;
		data_size_ = 0;
		body_length_ = 0;
	}

	// 返回数据指针.
	char* data()
	{
		return data_;
	}

	// 整个数据包的长度.
	size_t length() const
	{
		return HEADER_LENGTH + body_length_;
	}

	// 设置session指针.
	void setsession(const session_ptr& _session)
	{
		session_ = _session;
	}

	// 得到session指针.
	void getsession(session_ptr& _session)
	{
		_session = session_.lock();
	}

	// 返回body,除掉header的数据.
	char* body()
	{
		return wptr_;
	}

	// 返回header的长度.
	size_t header_length() const
	{
		return HEADER_LENGTH;
	}

	// 返回body的长度.
	size_t body_length() const
	{
		return body_length_;
	}

	// 设置body的长度.
	void body_length(size_t length)
	{
		if (length < 0)
			body_length_ = 0;
		body_length_ = length;
	}

	// 返回header的头指针.
	message::headerPtr head()
	{
		return msg_;
	}

	// 专用于解析接收的header数据.
	bool decode_header()
	{
		msg_ = (headerPtr)data_;
		header msg = *msg_;
		// 检查校检和.
		if (msg.checksum != (msg.type + msg.rand + msg.packsize))			
		{
			body_length_ = 0;
			return false;
		}
		// 计算实际数据长度. 公式: body_length_ = 数据包大小 - header_length.
		body_length_ = msg.packsize - HEADER_LENGTH;
		// 如果内存不够,重新分配大内存.
		if ((body_length_ + HEADER_LENGTH) > data_size_)
		{
			if (data_ != NULL)
				delete data_;

			data_size_ = body_length_ + HEADER_LENGTH;
			data_ = new char[data_size_];
			msg_ = (headerPtr)data_;
			*msg_ = msg;			
		}
		else if (((body_length_ + HEADER_LENGTH) < DEFAULT_BODY_LENGTH) && data_size_ > DEFAULT_BODY_LENGTH)
		{	// 撤消上次分配内存过大,重新分配内存.
			if (data_ != NULL)
				delete data_;

			data_size_ = DEFAULT_BODY_LENGTH + HEADER_LENGTH;
			data_ = new char[data_size_];
			msg_ = (headerPtr)data_;
			*msg_ = msg;
		}

		// 移动到数据开始位置.
		wptr_ = rptr_ = data_;
		wptr_ += HEADER_LENGTH;

		return true;
	}

	// 专用于检查接收body数据.
	bool check_body(size_t bytes_transferred)
	{
		msg_ = (headerPtr)data_;
		header msg = *msg_;
		if (bytes_transferred < body_length_) 
		{
			body_length_ -= bytes_transferred;
			wptr_ += bytes_transferred;
			return false; // 未读完整或者出错.
		}
		
		// 恢复写指针.
		wptr_ = data_ + HEADER_LENGTH;
		return true;
	}

private:
	headerPtr msg_;			// 指向数据头指针.
	char* data_;			// 数据指针.
	char* wptr_;			// 写数据指针.
	char* rptr_;			// 读数据指针.
	size_t data_size_;		// 内存块大小.
	size_t body_length_;	// 为实际数据长度.
	boost::weak_ptr<session> session_;
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
	message* message_;

#if defined(USE_SYNC)
	boost::asio::io_service::strand strand_;
#endif // USE_SYNC

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
	message* message_;	

#if defined(USE_SYNC)
	boost::asio::io_service::strand strand_;
#endif // USE_SYNC

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