#include <gal/net/server.hpp>

typedef gal::net::server THIS;

THIS::server()
{
}
void			THIS::connect(
		S_IO ios,
		const ip::tcp::endpoint& endpoint)
{
	printv_func(DEBUG);

	io_service_ = ios;
	assert(ios);

	// socket
	socket_.reset(new ip::tcp::socket(*ios));

	// acceptor
	printv(DEBUG, "create acceptor\n");


	//acceptor_.reset(new ip::tcp::acceptor(*io_service, endpoint));
	acceptor_.reset(new ip::tcp::acceptor(*ios));
	
	boost::asio::socket_base::reuse_address option(true);

	acceptor_->set_option(option);

	acceptor_->bind(endpoint);



	printv(DEBUG, "do accept\n");
	do_accept();
}
THIS::~server()
{
	printv_func(DEBUG);

	assert(!acceptor_);
}
void			THIS::release()
{
	printv_func(DEBUG);

	if(acceptor_) {
		acceptor_->cancel();
		acceptor_.reset();
	}
}
void			THIS::do_accept()
{
	printv_func(DEBUG);

	auto self(std::dynamic_pointer_cast<THIS>(shared_from_this()));

	auto ios = io_service_;//.lock();

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
		printv(ERROR, "accept error\n");
	}
	do_accept();
}
void			THIS::callback_accept(S_SOC socket)
{
	printv_func(DEBUG);

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

