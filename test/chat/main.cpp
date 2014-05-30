
#include <Galaxy-Network/server.hpp>

class communicating: public gal::net::communicating {
	public:
		communicating(int socket): gal::net::communicating(socket) {}
		void		process(sp::shared_ptr<gal::net::imessage> message) {
		}
};

class server: public gal::net::server {
	public:
		server(): gal::net::server(3000,10) {}
		virtual void		callback_accept(int socket) {

			clients_.push_back(sp::make_shared<communicating>(socket));

		}
		std::vector< sp::shared_ptr< communicating > > clients_;
};






int main() {

	auto serv = std::make_shared<server>();

}

