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

#include <Galaxy-Network/basic.hpp>
#include <Galaxy-Network/communicating.hpp>

namespace ip = boost::asio::ip;

namespace gal {
	namespace net {
		class server: public gal::net::__basic {
			public:
				typedef std::shared_ptr<gal::net::communicating>	comm_type;
				typedef std::shared_ptr<gal::net::message>		msg_type;
			public:
				server(
						boost::asio::io_service& io_service,
						const ip::tcp::endpoint& endpoint);
				
				
				virtual ~server();
				virtual void				callback_accept(ip::tcp::socket&& socket) = 0;
				void					write(sp::shared_ptr<omessage>);
				void					close();
			private:
				void					do_accept();
				void					thread_accept(boost::system::error_code);
			protected:
				boost::asio::io_service&		io_service_;
			private:
				ip::tcp::acceptor			acceptor_;
				ip::tcp::socket				socket_;
		};
	}
}




#endif



