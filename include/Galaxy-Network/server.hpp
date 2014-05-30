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

#include <Galaxy-Network/communicating.hpp>

namespace gal {
	namespace net {
		class server {
			public:
				typedef std::shared_ptr<gal::net::communicating>	comm_type;
				typedef std::shared_ptr<gal::net::message>		msg_type;
				
				server(unsigned short localPort, int queueLen);
				void			thread_accept();
				virtual void		callback_accept(int) = 0;
				void			write(sp::shared_ptr<omessage>);
				void			close();
			private:
				int					socket_;
				
				std::thread				thread_accept_;
				std::mutex				mutex_;

				unsigned short				local_port_;
			public:
				std::vector<comm_type>			clients_;
		};
	}
}




#endif



