#include <iostream>

#include <Galaxy-Network/server.hpp>

class communicating: public gal::net::communicating {
	public:
		communicating(boost::asio::io_service& io_service, ip::tcp::socket&& socket): gal::net::communicating(io_service, std::move(socket)) {}
	
		void		process(sp::shared_ptr<gal::net::imessage> message) {
			std::cout << "client: " << message->ss_.str() << std::endl;
		}
};

class server: public gal::net::server {
	public:
		server(boost::asio::io_service& io_service, ip::tcp::endpoint const & endpoint)
			: gal::net::server(io_service, endpoint) 
		{
		}
		virtual void		callback_accept(ip::tcp::socket&& socket) {
			auto clie = sp::make_shared<communicating>(io_service_, std::move(socket));
			clie->do_read_header();
			clients_.push_back(clie);
		}
	
		void			send_all(sp::shared_ptr<gal::net::omessage> omessage) {
			for(auto c : clients_) {
				c->write(omessage);
			}
		}
		std::vector< sp::shared_ptr< communicating > > clients_;
};






int main(int ac, char** av) {
	
	if(ac != 2) {
		std::cout << "usage: " << av[0] << " <port>" << std::endl;
		return 1;
	}

	boost::asio::io_service io_service;
	
	ip::tcp::endpoint endpoint(ip::tcp::v4(), std::atoi(av[1]));
	
	auto serv = std::make_shared<server>(io_service, endpoint);
	
	std::thread t([&io_service](){ io_service.run(); });

	std::string text;
	while(1) {
		std::cin >> text;

		auto message = std::make_shared<gal::net::omessage>();

		message->ar_ << text;

		serv->send_all(message);
	}
	
	io_service.run();
}

