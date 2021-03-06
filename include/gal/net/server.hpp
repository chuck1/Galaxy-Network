#ifndef __JESS_ASIO_NETWORK_SOCKET_SERVER_HPP__
#define __JESS_ASIO_NETWORK_SOCKET_SERVER_HPP__

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>

#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in

#include <boost/asio.hpp>

#include <gal/verb/Verbosity.hpp>
#include <gal/stl/map.hpp>

#include <gal/net/basic.hpp>
#include <gal/net/communicating.hpp>

namespace ip = boost::asio::ip;

namespace gal { namespace net {
	class server:
		virtual public gal::verb::Verbosity<gal::net::server>,
		virtual public gal::mng::managed_object
	{
		public:
			using gal::verb::Verbosity<gal::net::server>::printv;
			typedef gal::net::communicating				COM;
			typedef std::shared_ptr<COM>				S_COM;
			typedef std::shared_ptr<gal::net::message>		S_MSG;
			typedef std::shared_ptr<boost::asio::io_service>	S_IO;
			typedef std::weak_ptr<boost::asio::io_service>		W_IO;
			typedef std::shared_ptr<ip::tcp::socket>		S_SOC;
			typedef std::shared_ptr<ip::tcp::acceptor>		S_ACC;

			server();
			virtual ~server();
			void			release();
			void			connect(
					S_IO io_service,
					const ip::tcp::endpoint& endpoint);
			void			do_accept();
			void			close();
			void			thread_accept(boost::system::error_code ec);
			void			callback_accept(S_SOC socket);
			void			send_all(S_MSG msg);
			virtual void		accept(S_SOC &&) = 0;
		protected:
			W_IO			io_service_;
			S_ACC			acceptor_;
			S_SOC			socket_;
			//gal::stl::map<S_COM>				clients_;
	};
}}




#endif



