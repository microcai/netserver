#include "stdafx.h"
#include "net.hpp"

//////////////////////////////////////////////////////////////////////////

io_service_pool::io_service_pool(std::size_t pool_size)
: next_io_service_(0)
{
	if (pool_size == 0)
		throw std::runtime_error("io_service_pool size is 0");

	for (std::size_t i = 0; i < pool_size; ++i)
	{
		io_service_ptr io_service(new boost::asio::io_service);
		work_ptr work(new boost::asio::io_service::work(*io_service));
		io_services_.push_back(io_service);
		work_.push_back(work);
	}
}

void io_service_pool::run()
{
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for (std::size_t i = 0; i < io_services_.size(); ++i)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, io_services_[i])));
		threads.push_back(thread);
	}

	for (std::size_t i = 0; i < threads.size(); ++i)
		threads[i]->join();
}

void io_service_pool::stop()
{
	for (std::size_t i = 0; i < io_services_.size(); ++i)
		io_services_[i]->stop();
}

boost::asio::io_service& io_service_pool::get_io_service()
{
	boost::asio::io_service& io_service = *io_services_[next_io_service_];
	++next_io_service_;
	if (next_io_service_ == io_services_.size())
		next_io_service_ = 0;
	return io_service;
}

//////////////////////////////////////////////////////////////////////////
#if defined(SOCKET_SSL)

session::session(boost::asio::io_service& io_service, 
				jobqueue<message>& jobwork, 
				boost::object_pool<message>& message_pool,
				boost::asio::ssl::context& context)
: socket_(io_service, context)
#if defined(USE_SYNC)
, strand_(io_service)
#endif // USE_SYNC
, jobwork_(jobwork)
, message_pool_(message_pool)
{
	message_ = message_pool_.construct();
}

ssl_socket::lowest_layer_type& session::socket()
{
	return socket_.lowest_layer();
}

#else

session::session(boost::asio::io_service& io_service, jobqueue<message>& jobwork, boost::object_pool<message>& message_pool)
: socket_(io_service)
#if defined(USE_SYNC)
, strand_(io_service)
#endif // USE_SYNC
, jobwork_(jobwork)
, message_pool_(message_pool)
{
	message_ = message_pool_.construct();
}

tcp::socket& session::socket()
{
	return socket_;
}

#endif // SOCKET_SSL

session::~session()
{
}

#if defined(SOCKET_SSL)

void session::start()
{
	message_->setsession(shared_from_this());

	socket_.async_handshake(boost::asio::ssl::stream_base::server,
		boost::bind(&session::handle_handshake, shared_from_this(),
		boost::asio::placeholders::error));
}

void session::handle_handshake(const boost::system::error_code& error)
{
	if (!error)
	{
		socket_.async_read_some(boost::asio::buffer(message_->data(), message_->header_length()),
			
#if defined(USE_SYNC)
			strand_.wrap(
#endif // USE_SYNC
			boost::bind(&session::handle_read_head, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
#if defined(USE_SYNC)
			)
#endif // USE_SYNC
			);
	}
	else
	{
		message_pool_.destroy(message_);
	}
}

#else

void session::start()
{
	message_->setsession(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(message_->data(), message_->header_length()),
#if defined(USE_SYNC)
		strand_.wrap(
#endif // USE_SYNC
		boost::bind(&session::handle_read_head, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)
#if defined(USE_SYNC)
		)
#endif // USE_SYNC
		);
}

#endif // SOCKET_SSL

void session::handle_read_body(const boost::system::error_code& error,
							   size_t bytes_transferred)
{
	if (!error)
	{
		if (message_->check_body(bytes_transferred))
		{
			// 提交数据包到队列.
			jobwork_.submitjob(*message_);
			// 读取下一个数据包.
			socket_.async_read_some(boost::asio::buffer(message_->data(), message_->header_length()),
#if defined(USE_SYNC)
				strand_.wrap(
#endif // USE_SYNC
				boost::bind(&session::handle_read_head, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
#if defined(USE_SYNC)
				)
#endif // USE_SYNC
				);
		}
		else
		{
			// 数据包未读取完整,继续读取.
			socket_.async_read_some(boost::asio::buffer(message_->body(), message_->body_length()),
#if defined(USE_SYNC)
				strand_.wrap(
#endif // USE_SYNC
				boost::bind(&session::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
#if defined(USE_SYNC)
				)
#endif // USE_SYNC
				);
		}
	}
	else
	{
		message_pool_.destroy(message_);
	}
}

void session::handle_read_head(const boost::system::error_code& error,
				 size_t bytes_transferred)
{
	if (!error && message_->decode_header())
	{
		socket_.async_read_some(boost::asio::buffer(message_->body(), message_->body_length()),
#if defined(USE_SYNC)
			strand_.wrap(
#endif // USE_SYNC
			boost::bind(&session::handle_read_body, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
#if defined(USE_SYNC)
			)
#endif // USE_SYNC
			);
	}
	else
	{
		message_pool_.destroy(message_);
	}
}

void session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
	// 	socket_.async_read_some(boost::asio::buffer(data_, max_length),
	// 		boost::bind(&session::handle_read, shared_from_this(),
	// 		boost::asio::placeholders::error,
	// 		boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		// 优雅的关闭套接字.
		boost::system::error_code ignored_ec;
#if defined(SOCKET_SSL)
		socket_.shutdown(ignored_ec);
#else
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
#endif // SOCKET_SSL

		message_pool_.destroy(message_);
	}
}

//////////////////////////////////////////////////////////////////////////
#if defined(SOCKET_SSL)
server::server(short port, jobqueue<message>& jobwork, std::size_t io_service_pool_size/* = 4*/)
: io_service_pool_(io_service_pool_size)
, jobwork_(jobwork)
, acceptor_(io_service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
, context_(io_service_pool_.get_io_service(), boost::asio::ssl::context::sslv23)
{
	context_.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::single_dh_use);
	context_.set_password_callback(boost::bind(&server::get_password, this));
	context_.use_certificate_chain_file("server.pem");
	context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
	context_.use_tmp_dh_file("dh512.pem");

	session_ptr new_session(new session(io_service_pool_.get_io_service(), jobwork_, message_pool_, context_));
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

std::string server::get_password() const
{
	return "test";
}

void server::handle_accept(session_ptr new_session,
						   const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->start();
		new_session.reset(new session(io_service_pool_.get_io_service(), jobwork_, message_pool_, context_));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}
}

#else

server::server(short port, jobqueue<message>& jobwork, std::size_t io_service_pool_size/* = 4*/)
: io_service_pool_(io_service_pool_size)
, jobwork_(jobwork)
, acceptor_(io_service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
{
	session_ptr new_session(new session(io_service_pool_.get_io_service(), jobwork_, message_pool_));
	acceptor_.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
		boost::asio::placeholders::error));
}

void server::handle_accept(session_ptr new_session,
						   const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->start();
		new_session.reset(new session(io_service_pool_.get_io_service(), jobwork_, message_pool_));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}
}

#endif // SOCKET_SSL


void server::run()
{
	io_service_pool_.run();
}

void server::stop()
{
	io_service_pool_.stop();
}

