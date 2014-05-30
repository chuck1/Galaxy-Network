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


//#include <galaxy/config.hpp>
#include <Galaxy-Network/server.hpp>


gal::net::server::server(unsigned short localPort, int queueLen):
	socket_(::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)),
	thread_accept_(std::bind(&server::thread_accept, this)),
	local_port_(localPort)
{
	//GALAXY_DEBUG_0_FUNCTION;


}
gal::net::server::~server() {
	notify_bits(TERMINATE);

	if(thread_accept_.joinable())
		thread_accept_.join();
}
void gal::net::server::close() {
	notify_bits(TERMINATE);

	if(thread_accept_.joinable())
		thread_accept_.join();
}
void		gal::net::server::thread_accept() {
	//GALAXY_DEBUG_0_FUNCTION;

	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(local_port_);


	int c = bind (socket_, (struct sockaddr *)&addr, sizeof(addr));
	if(c < 0)
	{
		perror("bind:");  
	}

	listen(socket_,5);

	socklen_t len = sizeof(addr);
	
	while(1)
	{
		int s = ::accept(socket_, (struct sockaddr *)&addr, &len);
		if(s < 0) {
			perror("accept:");
			notify_bits(TERMINATE);
			return;
		}


		{
			//std::lock_guard<std::mutex> lk(mutex_);

			callback_accept(s);
		}
	}
}
/*void		gal::net::server::write(sp::shared_ptr<gal::net::omessage> msg) {

  for(auto it = clients_.begin(); it != clients_.end(); ++it) {
  (*it)->write(msg);
  }

  }*/


