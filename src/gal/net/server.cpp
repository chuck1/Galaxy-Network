#include <gal/net/server.hpp>

typedef gal::net::server THIS;

THIS::server()
{
}
void			THIS::connect(
		S_IO ios,
		const ip::tcp::endpoint& ep)
{
	printv_func(DEBUG);

	io_service_ = ios;
	assert(ios);

	// socket
	socket_.reset(new ip::tcp::socket(*ios));

	// acceptor
	printv(DEBUG, "create acceptor %i\n", ep.port());

	if(0) {
		acceptor_.reset(new ip::tcp::acceptor(*ios, ep.protocol()));
		acceptor_->bind(ep);
	} else if(1) {
		acceptor_.reset(new ip::tcp::acceptor(*ios, ep, true));
	}
	//
	
	//boost::asio::socket_base::reuse_address option(true);

	//acceptor_->set_option(option);


	printv(DEBUG, "do accept\n");
	do_accept();
}
THIS::~server()
{
	printv_func(DEBUG);

	assert(!acceptor_);
	assert(!socket_);
}
void			THIS::release()
{
	printv_func(DEBUG);

	if(acceptor_) {
		//acceptor_->cancel();
		//acceptor_->cancel();
		printv(DEBUG, "delete acceptor\n");
		acceptor_.reset();
	}

	if(socket_) {
		printv(DEBUG, "delete socket\n");
		socket_.reset();
	}
}
void			THIS::do_accept()
{
	printv_func(DEBUG);

	auto self(std::dynamic_pointer_cast<THIS>(shared_from_this()));

	auto ios = io_service_.lock();
	assert(ios);

	if(!socket_) socket_.reset(new ip::tcp::socket(*ios));

	boost::asio::socket_base::reuse_address option(true);

	acceptor_->set_option(option);

	printv(DEBUG, "async accept\n");
	acceptor_->async_accept(
			*socket_,
			boost::bind(
				&THIS::thread_accept,
				self,
				_1));

	printv(DEBUG, "done\n");
}
void			THIS::close()
{
	printv_func(DEBUG);

	acceptor_->cancel();
}
void			THIS::thread_accept(boost::system::error_code ec)
{
	printv_func(DEBUG);

	if(!ec) {	
		std::cout << "accepted" << ::std::endl;
		callback_accept(std::move(socket_));
	} else {
		printv(ERROR, "accept error: %s\n", ec.message().c_str());
	}
	do_accept();
}
void			THIS::callback_accept(S_SOC socket)
{
	printv_func(DEBUG);
	
	auto ios = io_service_.lock();
	assert(ios);
	
	auto cp = new COM;

	S_COM c(cp);

	c->connect(ios, std::move(socket));

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

