#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include <netinet/in.h>
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in

#include <gal/net/client.hpp>

//#include <galaxy/config.hpp>
//#include <galaxy/free.hpp>

void fillAddr(char const * address, unsigned short port, sockaddr_in &addr)
{
	memset(&addr, 0, sizeof(addr));  // Zero out address structure
	addr.sin_family = AF_INET;       // Internet address

	hostent *host;  // Resolve name

	if ((host = gethostbyname(address)) == NULL)
	{
		// strerror() will not work for gethostbyname() and hstrerror() 
		// is supposedly obsolete
		printf("error\n");
		throw;// SocketException("Failed to resolve name (gethostbyname())");
	}

	addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

	addr.sin_port = htons(port);     // Assign port in network byte order
}

typedef gal::net::client THIS;

void			THIS::connect(
		S_IO io_service,
		ip::tcp::resolver::iterator endpoint_iterator)
{
	do_connect(endpoint_iterator);
}
void			THIS::do_connect(
		ip::tcp::resolver::iterator endpoint_iterator)
{
	auto ios = io_service_.lock();
	assert(ios);

	if(!socket_)
		socket_.reset(new ip::tcp::socket(*ios));
	assert(socket_);

	boost::asio::async_connect(
			*socket_,
			endpoint_iterator,
			boost::bind(
				&THIS::thread_after_connect,
				shared_from_this(),
				_1,
				_2));

}
void			THIS::thread_after_connect(
		boost::system::error_code ec,
		ip::tcp::resolver::iterator)
{
	//printv_func(DEBUG);

	auto ios = io_service_.lock();

	auto self = shared_from_this();

	if (ec) {
		printf("connect failed\n");
		abort();
	}

	printv(INFO, "gal::net::client connected\n");

	// async
	do_read_header();
	// async
	launch_write_thread();

	// will wait for server desc
	after_connect();
}





