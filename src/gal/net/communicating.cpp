#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <boost/bind.hpp>

#include <gal/net/message.hpp>
#include <gal/net/barrier.hpp>

#include <gal/net/communicating.hpp>

typedef gal::net::communicating THIS;

//template<> int gal::verb::Verbosity<gal::net::communicating>::_M_level = DEBUG;

#define QUEUE_METHOD 1

THIS::communicating():
	_M_write_count(0)
{
}
THIS::communicating(THIS && c):
	io_service_(std::move(c.io_service_)),
	socket_(std::move(c.socket_)),
	_M_write_count(0)
{
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

	auto self = shared_from_this();

	io_service_ = io_service;
	
	reset_read_msg();
	//read_msg_.reset(new gal::net::message);
	//read_msg_->init_input();

	printv(DEBUG, "done\n");
}
void			THIS::connect(
		S_IO io_service)
{
	printv_func(DEBUG);

	socket_.reset(new ip::tcp::socket(*io_service));

	auto self = shared_from_this();

	io_service_ = io_service;

	reset_read_msg();
	//read_msg_.reset(new gal::net::message);
	//read_msg_->init_input();

}
void			THIS::launch()
{
	printv_func(DEBUG);
	auto self = shared_from_this();

	if(QUEUE_METHOD) {
		std::thread t(boost::bind(&THIS::do_write, self));
		t.detach();
	}
}
void			THIS::write(S_MSG msg)
{	
	printv_func(DEBUG);

	auto self = shared_from_this();

	if(QUEUE_METHOD) {
		if(!_M_flag.any(THIS::Flag::E::WRITE_THREAD_LAUNCHED)) {
			_M_flag.set(THIS::Flag::E::WRITE_THREAD_LAUNCHED);
			launch();
			_M_barrier_write_thread_launch.wait();
		}
	}
	
	if(QUEUE_METHOD) { // cv method
		//auto io = io_service_.lock();

		// wait for the write thread to wait on the cv
		{
			std::lock_guard<std::mutex> lk(_M_mutex_write_queue);
			write_msgs_.push_back(msg);
		}
		printv(DEBUG, "notifying\n");
		_M_cv_write.notify_one();

	} else { // on demand method


		printv(DEBUG, "on demand\n");
		printv(DEBUG, "lock write mutex\n");
		_M_mutex_write.lock();
		printv(DEBUG, "write mutex locked\n");

		std::string str(msg->ss_.str());

		header_type header(str.size(), _M_write_count++);

		printv(DEBUG, "write: header = %i %i\n", header._M_size, header._M_uid);

		printv(DEBUG, "async_write: %lu\n", sizeof(header_type));
		// non-blocking
		boost::asio::async_write(
				*socket_,
				boost::asio::buffer(&header, sizeof(header_type)),
				boost::bind(
					&gal::net::communicating::thread_do_write_header,
					self,
					_1,
					_2,
					msg));

	}
}
void			THIS::launch_write_thread()
{
	printv_func(DEBUG);
	if(0) {
		auto ios = io_service_.lock();
		assert(ios);

		auto self = shared_from_this();

		ios->post(boost::bind(&THIS::do_write, self));
	}
}
/*
   void		gal::net::communicating::thread_write(S_MSG msg0)
   {
   printv_func(DEBUG);

   printf("lock write utex\n");
   if(_M_utex_write.try_lock()) {
   printf("write utex is locked\n");

//bool write_in_progress = !write_msgs_.empty();


//if(!write_in_progress) {
do_write();//msg1);
//}
} else {
printf("could not lock write utex\n");
}
}
*/
void		gal::net::communicating::thread_write(S_MSG msg)
{
	printv_func(DEBUG);

	// make sure header and body are writen before starting another write
	printv(DEBUG, "thread write: write mutex lock\n");
	_M_mutex_write.lock();
	printv(DEBUG, "thread write: write mutex lock aquired\n");

	std::string str(msg->ss_.str());

	header_type header(str.size(), _M_write_count++);

	auto self = shared_from_this();

	// non-blocking
	// sets up a sequence of async calls and callbacks
	// at the end, the mutex_write is unlocked
	printf("async write: header = {uid=%i, size=%i}, sizeof(header_type) = %lu\n",
			header._M_uid,
			header._M_size,
			sizeof(header_type));

	boost::asio::async_write(
			*socket_,
			boost::asio::buffer(&header, sizeof(header_type)),
			boost::bind(
				&gal::net::communicating::thread_do_write_header,
				self,
				_1,
				_2,
				msg));

}
void		gal::net::communicating::do_write(/*S_MSG msg*/)
{
	printv_func(DEBUG);

	auto self = shared_from_this();

	std::unique_lock<std::mutex> lk(_M_mutex_write_queue);

	_M_barrier_write_thread_launch.wait();

	S_MSG msg;

	while(1) {
		// allow other threads to add messages to queue
		printv(DEBUG, "do_write: wait\n");
		_M_cv_write.wait(lk);
		printv(DEBUG, "do_write: notified\n");

		//{
		//std::lock_guard<std::utex> lk(_M_utex_write_queue);
		msg = write_msgs_.front();
		write_msgs_.pop_front();
		//}

		//printf("DEBUG: sending message of length %i\n", (int)str.size());

		if(1) {
			std::thread t(boost::bind(
				&gal::net::communicating::thread_write,
				self,
				msg));

			t.detach();
			//return;

		} else if(0) {
/*
			// blocking
			printf("blocking write: %lu\n", sizeof(header_type));
			boost::asio::write(
					*socket_,
					boost::asio::buffer(&header, sizeof(header_type)));

			std::string str(msg->ss_.str());

			printf("blocking write: %lu\n", str.size());
			boost::asio::write(
					*socket_,
					boost::asio::buffer(str.c_str(), str.size()));

*/
		} else if(0) {

/*
			boost::asio::async_write(
					*socket_,
					boost::asio::buffer(&header, sizeof(header_type)),
					boost::bind(
						&gal::net::communicating::thread_do_write_header,
						self,
						_1,
						_2,
						msg));
*/
			return;
		}

	}
}
void			gal::net::communicating::thread_do_write_header(
		boost::system::error_code ec,
		size_t length,
		S_MSG msg)
{
	printv_func(DEBUG);

	if(ec) {
		printf("thread_do_write_header error: %s\n", ec.message().c_str());
		abort();
	}

	std::string str(msg->ss_.str());

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	printv(DEBUG, "async write: %lu\n", str.size());

	boost::asio::async_write(
			*socket_,
			boost::asio::buffer(str.c_str(), str.size()),
			boost::bind(
				&gal::net::communicating::thread_do_write_body,
				self,
				_1,
				_2));
}
void			gal::net::communicating::thread_do_write_body(
		boost::system::error_code ec,
		size_t length)
{
	printv_func(DEBUG);

	if(ec) {
		printv(CRITICAL, "error: %s\n", ec.message().c_str());
		socket_->close();
		abort();
	}

	printv(DEBUG, "write successful\n");
	_M_mutex_write.unlock();
	printv(DEBUG, "write mutex unlocked\n");

	//do_write();
	/*
	   std::lock_guard<std::utex> lk(_M_utex_write_queue);
	   bool empty = write_msgs_.empty();

	   if(empty) {
	   printf("unlock write utex\n");
	//_M_utex_write.unlock();
	return;
	} else {
	do_write();
	}

	_M_utex_write.unlock();
	*/
}
void			THIS::close()
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
  std::lock_guard<std::utex> lk(utex_);
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
void			THIS::do_read_header()
{
	printv_func(DEBUG);

	auto self = shared_from_this();

	assert(socket_);
	assert(socket_->is_open());

	printv(DEBUG, "async read: %lu\n", sizeof(header_type));
	boost::asio::async_read(
			*socket_,
			boost::asio::buffer(&read_header_, sizeof(header_type)),
			boost::bind(
				&THIS::thread_read_header,
				self,
				_1,
				_2));

}
void			THIS::thread_read_header(
		boost::system::error_code ec,
		size_t length)
{
	printv_func(DEBUG);

	if(ec) {
		printv(ERROR, "%s : %s\n", __PRETTY_FUNCTION__, ec.message().c_str());

		if(
				(ec.value() == boost::asio::error::eof) ||
				(ec.value() == boost::asio::error::connection_reset)) {
			printv(ERROR, "closing connection\n");
			close();
			return;
		}

		assert(0);
	}

	if(length != sizeof(header_type)) {
		printf("%s : unknwon error\n", __PRETTY_FUNCTION__);
		abort();
	}

	//printv(DEBUG, "read header = %i\n", read_header_);

	do_read_body();
}
void	gal::net::communicating::do_read_body()
{
	printv_func(DEBUG);

	auto self(std::dynamic_pointer_cast<gal::net::communicating>(shared_from_this()));

	printv(DEBUG, "async read: header = %i %i\n", read_header_._M_size, read_header_._M_uid);

	boost::asio::async_read(*socket_,
			boost::asio::buffer(read_buffer_, read_header_._M_size),
			boost::bind(
				&gal::net::communicating::thread_read_body,
				self,
				_1,
				_2)
			);
}
void			THIS::thread_read_body(
		boost::system::error_code ec,
		size_t)
{
	printv_func(DEBUG);

	if(ec) {
		printv(ERROR, "%s : %s\n", __PRETTY_FUNCTION__, ec.message().c_str());

		if(ec.value() == boost::asio::error::eof) {
			printv(ERROR, "closing connection\n");
			close();
			return;
		}

		assert(0);
	}

	reset_read_msg();

	// process message
	read_msg_->reset_head();
	read_msg_->ss_.write(read_buffer_, read_header_._M_size);

	// call pure virtual function to process data in message
	//assert(_M_process_func);
	printv(DEBUG, "call process func\n");
	//_M_process_func(read_msg_);
	v_process(read_msg_);

	// restart read process
	do_read_header();
}



