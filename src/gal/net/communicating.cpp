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

template<> int gal::tmp::Verbosity<gal::net::communicating>::_M_level = DEBUG;

THIS::communicating()
{
	printv_func(DEBUG);
}
THIS::communicating(THIS && c):
	socket_(std::move(c.socket_)),
	io_service_(std::move(c.io_service_))
{
	printv_func(DEBUG);
}
THIS&			THIS::operator=(communicating && c)
{
	socket_ = std::move(c.socket_);
	io_service_ = std::move(c.io_service_);
	return *this;
}
THIS::~communicating()
{
	assert(!socket_);
}
void			THIS::connect(
		S_IO io_service,
		S_SOC socket)
{
	printv_func(DEBUG);

	socket_ = std::move(socket);

	io_service_ = io_service;

	printv(DEBUG, "read msg\n");
	read_msg_.reset(new gal::net::message);
	//read_msg_->init_input();

	printv(DEBUG, "done\n");
}
void			THIS::connect(S_IO io_service)
{
	printv_func(DEBUG);

	socket_.reset(new ip::tcp::socket(*io_service));

	io_service_ = io_service;

	read_msg_.reset(new gal::net::message);
	//read_msg_->init_input();
}
void		gal::net::communicating::do_read_header()
{
	printv_func(DEBUG);

	auto self = shared_from_this();

	assert(self);

	assert(socket_);

	assert(socket_->is_open());

	boost::asio::async_read(*socket_,
			boost::asio::buffer(&read_header_, sizeof(header_type)),
			boost::bind(
				&gal::net::communicating::thread_read_header,
				self,
				_1,
				_2)
			);

}
void			gal::net::communicating::write(
		S_MSG msg)
{	
	printv_func(DEBUG);

	auto self = shared_from_this();

	auto io = io_service_.lock();
	
	io->post(boost::bind(&gal::net::communicating::thread_write, self, msg));
}
void		gal::net::communicating::thread_write(S_MSG msg)
{
	printv_func(DEBUG);

	bool write_in_progress = !write_msgs_.empty();

	write_msgs_.push_back(msg);

	if(!write_in_progress) {
		do_write();
	}
}
void		gal::net::communicating::do_write() {

	printv_func(DEBUG);

	auto msg = write_msgs_.front();
	write_msgs_.pop_front();

	std::string str(msg->ss_.str());

	//printf("DEBUG: sending message of length %i\n", (int)str.size());

	header_type header = str.size();

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_write(*socket_,
			boost::asio::buffer(&header, sizeof(header_type)),
			boost::bind(
				&gal::net::communicating::thread_do_write_header,
				self,
				_1,
				_2,
				msg)
			);
}
void			gal::net::communicating::thread_do_write_header(
		boost::system::error_code ec,
		size_t length,
		S_MSG msg)
{
	printv_func(DEBUG);

	std::string str(msg->ss_.str());

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_write(*socket_,
			boost::asio::buffer(str.c_str(), str.size()),
			boost::bind(
				&gal::net::communicating::thread_do_write_body,
				self,
				_1,
				_2)
			);
}
void			gal::net::communicating::thread_do_write_body(
		boost::system::error_code ec,
		size_t length)
{
	printv_func(DEBUG);

	if (!ec) {
		if (!write_msgs_.empty()) {
			do_write();
		}
	} else {
		printf(/*ERRRO,*/ "socket error\n");
		socket_->close();
	}
}
void	gal::net::communicating::close()
{
	printv_func(DEBUG);

	if(socket_) {
		auto io = io_service_.lock();
		io->post([this]() {
				socket_->close();
				socket_.reset();
				});
	}
}
void			THIS::release()
{
	printv_func(DEBUG);
	
	close();
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
void			THIS::thread_read_header(
		boost::system::error_code ec,
		size_t length)
{
	printv_func(DEBUG);

	if(ec) {
		printf("%s : %s\n", __PRETTY_FUNCTION__, ec.message().c_str());
		abort();
	}

	if(length != sizeof(header_type)) {
		printf("%s : unknwon error\n", __PRETTY_FUNCTION__);
		abort();
	}

	do_read_body();
}
void	gal::net::communicating::do_read_body()
{
	printv_func(DEBUG);

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	boost::asio::async_read(*socket_,
			boost::asio::buffer(read_buffer_, read_header_),
			boost::bind(&gal::net::communicating::thread_read_body, self, _1, _2)
			);
}
void			gal::net::communicating::thread_read_body(
		boost::system::error_code ec,
		size_t)
{
	printv_func(DEBUG);

	if (!ec) {

		if(1) {//if(!read_msg_) {
			printf("reset read message\n");
			read_msg_.reset(new gal::net::message);
		}

		// process message
		read_msg_->reset_head();
		read_msg_->ss_.write(read_buffer_, read_header_);

		// call pure virtual function to process data in message
		if(!_M_process_func) abort();
		printv(DEBUG, "call process func\n");
		_M_process_func(read_msg_);
		
		// restart read process
		do_read_header();
	} else {
		// error
		printf("%s: error\n", __PRETTY_FUNCTION__);
		return;
	}

}




