#ifndef __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__
#define __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__

#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <memory>

#include <sys/socket.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace ip = boost::asio::ip;

#include <gal/shared.hpp>
#include <gal/verb/Verbosity.hpp>
#include <gal/macro/Flag.hpp>

#include <gal/net/barrier.hpp>
#include <gal/net/decl.hpp>
#include <gal/net/basic.hpp>
#include <gal/net/message.hpp>

namespace gal { namespace net {
	/** @brief %communicating
	*/
	class communicating:
		virtual public gal::verb::Verbosity<gal::net::communicating>,
		virtual public gal::enable_shared_from_this<gal::net::communicating>
	{
	public:
		typedef gal::net::communicating COM;

		using gal::verb::Verbosity<COM>::printv;
		using gal::enable_shared_from_this<COM>::shared_from_this;
		
		DEFINE_FLAG(Flag,
				((WRITE_THREAD_LAUNCHED)(1<<0))
				)

		class Header
		{
		public:
			Header(int size, int uid):
				_M_size(size),
				_M_uid(uid)
			{}
			Header():
				_M_size(-1),
				_M_uid(-1)
			{}
			int	_M_size;
			int	_M_uid;
		};

		typedef Header						header_type;
		typedef std::shared_ptr<boost::asio::io_service>	S_IO;
		typedef std::weak_ptr<boost::asio::io_service>		W_IO;
		typedef std::shared_ptr<ip::tcp::socket>		S_SOC;
		typedef std::shared_ptr<gal::net::message>		S_MSG;
		enum { MAX_MESSAGE_LENGTH = 10000 };
		communicating();
		virtual ~communicating();
		/** @brief ctor
		 *
		 * @param socket socket
		 */
		void			connect(
				S_IO io_service,
				S_SOC socket);
		/** @brief ctor
		 *
		 * @param io_serive io_service
		 */
		void			connect(
				S_IO io_service);
	protected:
		/** @brief write
		 *
		 * send %message to socket
		 * @param %message %message to send
		 */
		virtual void		write(S_MSG message);
	public:
		/** @brief close
		 *
		 * close the socket and terminate threads
		 */
		void				close();
		void				release();
		communicating&			operator=(communicating &&);
	private:
		communicating(communicating const &) = default;
		communicating&			operator=(communicating const &) = default;
	protected:
		communicating(communicating &&);
		/*
		 * user-supplied function to process incomming messages
		 * 
		 * using std::function instead of pure virtual function so 
		 * that gal::net::server can instantiate instances of gal::net::communicating
		 * without being a template class
		 *
		 * future me: I dont understand the above, trying pure virt
		 */
		//std::function<void(S_MSG)>	_M_process_func;
		virtual void		v_process(S_MSG) = 0;
	public:
		void			launch_write_thread();
		void			launch();
		void			do_read_header();
		void			do_write(/*S_MSG*/);
	protected:
		void			thread_write(
				std::shared_ptr<gal::net::message> msg);
		void			thread_do_write_header(
				boost::system::error_code ec,
				size_t length,
				std::shared_ptr<gal::net::message> msg);
		void			thread_do_write_body(
				boost::system::error_code ec,
				size_t length);
		void			handle_do_write();
		void			thread_read();
		void			thread_read_header(
				boost::system::error_code ec, size_t length);
		void			do_read_body();
		void			thread_read_body(boost::system::error_code ec, size_t);
		virtual void		reset_read_msg() = 0;	
		virtual bool		is_open();
	public:
		W_IO					io_service_;
	protected:
		S_SOC					socket_;
		S_MSG					read_msg_;
	private:
		Flag					_M_flag;
		/** @name Read Data Members @{ */
		header_type				read_header_;
		char					read_buffer_[MAX_MESSAGE_LENGTH];
		/** @} */
		header_type				write_header_;
		/** message deque
		*/
		std::deque<S_MSG>			write_msgs_;
		/** process body
		*/
		/** thread write
		*/
		std::thread				write_thread_;
		/** thread read
		*/
		std::thread				read_thread_;
		std::condition_variable			_M_cv_write;
		std::mutex				_M_mutex_write_queue;
		std::mutex				_M_mutex_write;
		std::mutex				_M_mutex_write_thread_launch;
	
		gal::net::Barrier<2>			_M_barrier_write_thread_launch;
	
		int					_M_write_count;
	};
}}

#endif

