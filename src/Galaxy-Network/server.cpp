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

#include <boost/bind.hpp>

//#include <galaxy/config.hpp>
#include <Galaxy-Network/server.hpp>


gal::net::server::server(
		boost::asio::io_service& io_service,
		const ip::tcp::endpoint& endpoint):
	io_service_(io_service),
	acceptor_(io_service, endpoint),
	socket_(io_service)
{
	do_accept();
}
void		gal::net::server::do_accept() {

	auto self(sp::dynamic_pointer_cast<gal::net::server>(shared_from_this()));

	acceptor_.async_accept(
			socket_,
			boost::bind(
				&gal::net::server::thread_accept,
				self,
				_1
				)
			);

}
gal::net::server::~server() {
	acceptor_.cancel();
}
void gal::net::server::close() {
	acceptor_.cancel();
}
void		gal::net::server::thread_accept(boost::system::error_code ec) {

	if(!ec) {	
		::std::cout << "accepted" << ::std::endl;
		callback_accept(::std::move(socket_));
	}
	do_accept();
}
void		gal::net::server::callback_accept(ip::tcp::socket&& socket) {
	auto clie = sp::make_shared<T>(io_service_, ::std::move(socket));
	clie->init();
	clients_.insert(clie());
}



