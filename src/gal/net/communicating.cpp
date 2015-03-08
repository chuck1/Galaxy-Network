#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <boost/bind.hpp>

#include <gal/net/message.hpp>

#include <gal/net/communicating.hpp>

typedef gal::net::communicating THIS;

void			THIS::connect(
		S_IO io_service,
		ip::tcp::socket&& socket)
{
	socket_ = std::move(socket);

	io_service_ = io_service;

	read_msg_.reset(new gal::net::imessage);
}
void			THIS::connect(S_IO io_service)
{
	socket_ = ip::tcp::socket(*io_service);

	io_service_ = io_service;

	read_msg_.reset(new gal::net::imessage);
}
void		gal::net::communicating::do_read_header() {

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_read(socket_,
			boost::asio::buffer(&read_header_, sizeof(header_type)),
			boost::bind(&gal::net::communicating::thread_read_header, self, _1, _2)
			);

}
void		gal::net::communicating::write(std::shared_ptr<gal::net::omessage> msg) {	

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	auto io = io_service_.lock();
	
	io->post(boost::bind(&gal::net::communicating::thread_write, self, msg));
}
void		gal::net::communicating::thread_write(std::shared_ptr<gal::net::omessage> msg) {

	bool write_in_progress = !write_msgs_.empty();

	write_msgs_.push_back(msg);

	if(!write_in_progress) {
		do_write();
	}
}
void		gal::net::communicating::do_write() {

	auto msg = write_msgs_.front();
	write_msgs_.pop_front();

	std::string str(msg->ss_.str());

	//printf("DEBUG: sending message of length %i\n", (int)str.size());

	header_type header = str.size();

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_write(socket_,
			boost::asio::buffer(&header, sizeof(header_type)),
			boost::bind(
				&gal::net::communicating::thread_do_write_header,
				self,
				_1,
				_2,
				msg)
			);
}
void		gal::net::communicating::thread_do_write_header(boost::system::error_code ec, size_t length, std::shared_ptr<gal::net::omessage> msg) {

	::std::string str(msg->ss_.str());

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_write(socket_,
			boost::asio::buffer(str.c_str(), str.size()),
			boost::bind(
				&gal::net::communicating::thread_do_write_body,
				self,
				_1,
				_2)
			);
}
void		gal::net::communicating::thread_do_write_body(boost::system::error_code ec, size_t length) {
	if (!ec) {
		if (!write_msgs_.empty()) {
			do_write();
		}
	} else {
		printf(/*ERRRO,*/ "socket error\n");
		socket_.close();
	}
}
void	gal::net::communicating::close()
{
	auto io = io_service_.lock();
	
	io->post([this]() { socket_.close(); });
}
/*void		gal::net::communicating::notify_bits(unsigned int bits) {
	{
		std::lock_guard<std::mutex> lk(mutex_);
		bits_ |= bits;
	}
	cv_.notify_all();
}*/
/*void		gal::net::communicating::thread_write_body(boost::system::error_code ec, size_t length, std::shared_ptr<gal::net::omessage> message) {

	std::string str(message->ss_.str());

	int result = boost::asio::write(socket_, boost::asio::buffer(str.c_str(), str.size()));

	if(result < 0) {
		perror("send:");
		notify_bits(TERMINATE & ERROR);
		return;
	}

	if(result < (int)str.size()) {
		printf("unknown error\n");
		notify_bits(TERMINATE & ERROR);
		return;
	}
}*/
void	gal::net::communicating::thread_read_header(boost::system::error_code ec, size_t length) {

	if(ec) {
		printf("%s : %s\n", __PRETTY_FUNCTION__, ec.message().c_str());
		return;
	}

	if(length != sizeof(header_type)) {
		printf("%s : unknwon error\n", __PRETTY_FUNCTION__);
	}

	do_read_body();
}
void	gal::net::communicating::do_read_body() {

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_read(socket_,
			boost::asio::buffer(read_buffer_, read_header_),
			boost::bind(&gal::net::communicating::thread_read_body, self, _1, _2)
			);

}
void	gal::net::communicating::thread_read_body(boost::system::error_code ec, size_t) {

	if (!ec) {
		// process message
		read_msg_->reset_head();
		read_msg_->ss_.write(read_buffer_, read_header_);
		process(read_msg_);

		// restart read process
		do_read_header();
	} else {
		// error
		printf("%s: error\n", __PRETTY_FUNCTION__);
		return;
	}

}




