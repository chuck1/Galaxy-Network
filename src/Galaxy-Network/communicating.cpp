#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include <math/free.hpp>

//#include <galaxy/config.hpp>
//#include <gal/free.hpp>

#include <Galaxy-Network/message.hpp>
#include <Galaxy-Network/communicating.hpp>

gal::net::communicating::communicating( int socket ):
	socket_(socket),
	read_msg_(new gal::net::imessage)
{
	//GALAXY_DEBUG_0_FUNCTION;

	printf("socket = %i\n", socket);
}
void	gal::net::communicating::start() {

	std::unique_lock<std::mutex> lk(mutex_start_);

	write_thread_ = std::thread(std::bind(&communicating::thread_write_dispatch, this ) );

	cv_ready_.wait(lk);

	read_thread_ = std::thread(std::bind(&communicating::thread_read, this ) );
}
void		gal::net::communicating::write(sp::shared_ptr<gal::net::omessage> msg) {	

	{
		std::lock_guard<std::mutex> lk(mutex_);

		// take ownership of the message
		write_queue_.push_back(std::move(msg));
	}

	cv_.notify_one();
}
void	gal::net::communicating::close() {	

	notify_bits(TERMINATE);

	write_thread_.join();
	read_thread_.join();

	::close(socket_);
}
void		gal::net::communicating::notify_bits(unsigned int bits) {
	{
		std::lock_guard<std::mutex> lk(mutex_);
		bits_ |= bits;
	}
	cv_.notify_all();
}
void	gal::net::communicating::thread_write_dispatch() {
	//GALAXY_DEBUG_1_FUNCTION;

	//printf("lock mutex_start_\n");

	mutex_start_.lock();

	//printf("unlock mutex_start_\n");
	mutex_start_.unlock();

	//printf("lock mutex_\n");
	std::unique_lock<std::mutex> lk(mutex_);

	//printf("cv_ready_.notify_all()\n");
	cv_ready_.notify_all();

	while ( 1 )
	{
		//printf("wait\n");

		do {
			cv_.wait(lk);
		} while(write_queue_.empty() && !(bits_ & TERMINATE));


		//printf("notified\n");

		//cv_.wait( lk, [&] { return ( !write_queue_.empty() || terminate_ ); } );

		if(bits_ & TERMINATE) {
			//printf("terminated\n");
			return;
		}		

		//printf("create write thread\n");

		std::thread t(
				std::bind(
					&gal::net::communicating::thread_write,
					this,
					write_queue_.front()
					)
			     );
		t.detach();

		write_queue_.pop_front();
	}
}
void		gal::net::communicating::thread_write(sp::shared_ptr<gal::net::omessage> message) {

	std::string str(message->ss_.str());

	printf("DEBUG: sending message of length %i\n", (int)str.size());
	
	header_type header = str.size();
	
	int result = ::send(socket_, &header, sizeof(header_type), 0);

	if ( result < 0 ) {
		perror("send:");
		notify_bits(TERMINATE & ERROR);
		return;
	}

	if(result < (int)sizeof(header_type)) {
		printf("unknown error\n");
		notify_bits(TERMINATE & ERROR);
		return;
	}

	result = ::send(socket_, str.c_str(), str.size(), 0);

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
}
void	gal::net::communicating::thread_read() {
	//GALAXY_DEBUG_1_FUNCTION;

	while (1) {
		{
			std::lock_guard<std::mutex> lk( mutex_ );
			if(bits_ & TERMINATE) return;
		}

		try {
			thread_read_header();
		} catch(...) {
			notify_bits(TERMINATE & ERROR);
			return;
		}
	}
}
void	gal::net::communicating::thread_read_header() {
	//GALAXY_DEBUG_1_FUNCTION;

	//if ( !socket_->is_open() ) exit(0);//gal::cerr << "SOCKET NOT OPEN" << endl;


	//printf("waiting for %i bytes\n", message::header_length);

	// wail until all data is available
	int bytes = ::recv(socket_, &read_header_, sizeof(header_type), MSG_WAITALL);

	if (bytes < 0) {
		perror("recv:");
		throw 0;
	}

	if (bytes == 0) {
		printf("connection is closed\n");
		throw 0;
	}

	if (bytes < (int)sizeof(header_type)) {
		printf("%s\n", __PRETTY_FUNCTION__);
		printf("not enough data\n");
		throw 0;
	}

	handle_do_read_header();
}
void	gal::net::communicating::thread_read_body() {
	//GALAXY_DEBUG_1_FUNCTION;

	// wail until all data is available
	int bytes = ::recv(socket_, read_buffer_, read_header_, MSG_WAITALL);

	if(bytes < 0)
	{
		perror("recv:");
		throw 0;
	}


	if(bytes == 0) {
		printf("connection is closed\n");
		throw 0;
	}

	if(bytes < (int)sizeof(header_type)) {
		printf("not enough data\n");
		throw 0;
	}

	printf("DEBUG: received %i bytes\n", read_header_);
	//math::hexdump(read_msg_->body(), read_msg_->body_length());

	handle_do_read_body();
}

void	gal::net::communicating::handle_do_read_header() {
	//GALAXY_DEBUG_1_FUNCTION;

	if (read_msg_) {
		try {
			thread_read_body();
		} catch(...) {
			notify_bits(TERMINATE & ERROR);
			return;
		}
	} else {
		printf("header decode failed\n");
		notify_bits(TERMINATE & ERROR);
		return;
	}
}
void	gal::net::communicating::handle_do_read_body() {
	//GALAXY_DEBUG_1_FUNCTION;

	//gal::net::message_s msg(new gal::net::message);

	// copy buffer to message

	read_msg_->reset_head();

	read_msg_->ss_.write(read_buffer_, read_header_);

	process(read_msg_);
	try {
		thread_read_header();
	} catch(...) {
		notify_bits(TERMINATE & ERROR);
		return;
	}
}



