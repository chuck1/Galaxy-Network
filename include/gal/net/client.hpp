#ifndef __JESS_ASIO_NETWORK_SOCKET_CLIENT_HPP__
#define __JESS_ASIO_NETWORK_SOCKET_CLIENT_HPP__

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>

#include <gal/net/communicating.hpp>

//typedef std::shared_ptr<boost::asio::io_service>	boost_io_service_ptr;

namespace gal { namespace net {
	/// socket_client
	class client:
		virtual public gal::enable_shared_from_this<gal::net::client>,
		virtual public gal::net::communicating
	{
	public:
		using gal::enable_shared_from_this<gal::net::client>::shared_from_this;
		typedef std::shared_ptr<client>	shared_t;
		typedef std::shared_ptr<boost::asio::io_service>	S_IO;
		typedef std::weak_ptr<boost::asio::io_service>		W_IO;
		/// ctor
		void			connect(
				S_IO io_service,
				ip::tcp::resolver::iterator endpoint_iterator);
		/// write
		//void	write(gal::asio::message::shared_t msg);
		/// close
		//void	close();
	private:
		/// do_connect
		void			do_connect(ip::tcp::resolver::iterator endpoint_iterator);
		void			thread_after_connect(
				boost::system::error_code ec,
				ip::tcp::resolver::iterator);

		/// handle_do_connect
		//void	handle_do_connect();
	};
}}

#endif

