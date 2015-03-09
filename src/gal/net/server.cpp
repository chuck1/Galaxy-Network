#include <gal/net/server.hpp>

typedef gal::net::server THIS;

void			THIS::connect(
		S_IO io_service,
		const ip::tcp::endpoint& endpoint)
{
	io_service_ = io_service;
	acceptor_ = ip::tcp::acceptor(*io_service, endpoint);
	socket_.reset(new ip::tcp::socket(*io_service));
}
THIS::~server()
{
	acceptor_.cancel();
}
void			THIS::do_accept()
{
	auto self(std::dynamic_pointer_cast<THIS>(shared_from_this()));

	acceptor_.async_accept(
			*socket_,
			boost::bind(
				&THIS::thread_accept,
				self,
				_1));
}
void			THIS::close()
{
	acceptor_.cancel();
}
void			THIS::thread_accept(boost::system::error_code ec)
{
	if(!ec) {	
		std::cout << "accepted" << ::std::endl;
		callback_accept(std::move(socket_));
	}
	do_accept();
}
void			THIS::callback_accept(S_SOC socket)
{
	auto cp = new COM;
	
	S_COM c(cp);

	c->connect(io_service_, std::move(socket));
	
	accept(c);
	
	//clie->init();
	//clients_.insert(clie);
}
/*void			THIS::send_all(S_MSG omessage)
{
	for(auto c : clients_) {
		c.ptr_->write(omessage);
	}
}*/

