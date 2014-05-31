#ifndef __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__
#define __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__

#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <thread>
#include <condition_variable>

#include <sys/socket.h>

#include <boost/asio.hpp>

namespace ip = boost::asio::ip;

#include <Galaxy-Network/Types.hpp>
#include <Galaxy-Network/basic.hpp>
#include <Galaxy-Network/message.hpp>

namespace gal {
	namespace net {
		/** %socket %communicating
		 */
		class communicating: public gal::net::__basic, public std::enable_shared_from_this<communicating> {
			public:
				typedef int				header_type;
				enum { MAX_MESSAGE_LENGTH = 10000 };
			public:
				/** @brief ctor
				 *
				 * @param socket socket
				 */
				communicating(boost::asio::io_service& io_service, ip::tcp::socket&& socket);
				/** @brief ctor
				 *
				 * @param io_serive io_service
				 */
				communicating(boost::asio::io_service& io_service);
				/** @brief write
				 *
				 * send %message to socket
				 * @param %message %message to send
				 */
				void					write(sp::shared_ptr<omessage> message);
				/** @brief close
				 *
				 * close the socket and terminate threads
				 */
				void					close();
			private:
				communicating(communicating const &) = default;
				communicating(communicating &&) = default;
				communicating&				operator=(communicating const &) = default;
				communicating&				operator=(communicating &&) = default;

				virtual void				process(sp::shared_ptr<gal::net::imessage>) = 0;
			public:
				void					do_read_header();
			private:
				void					do_write();
				void					thread_write(sp::shared_ptr<gal::net::omessage> msg);
				void					thread_do_write_header(boost::system::error_code ec, std::size_t length, sp::shared_ptr<gal::net::omessage> msg);
				void					thread_do_write_body(boost::system::error_code ec, std::size_t length);

				/** thread write
				*/
				void					thread_write_body(boost::system::error_code ec, size_t length, sp::shared_ptr<gal::net::omessage>);
				
				/** @brief thread read
				*/
				void					thread_read();
				/** @brief thread read header
				*/
				void					thread_read_header(boost::system::error_code ec, size_t length);
				void					do_read_body();
				/** @brief thread read body
				*/
				void					thread_read_body(boost::system::error_code ec, std::size_t);
				/** @brief handle write
				*/
				void					handle_do_write();
			private:
				void					notify_bits(unsigned int bits);
			protected:
				ip::tcp::socket					socket_;
				boost::asio::io_service&			io_service_;
			private:
				/** @name Read Data Members @{ */
				sp::shared_ptr<gal::net::imessage>		read_msg_;
				header_type					read_header_;
				char						read_buffer_[MAX_MESSAGE_LENGTH];
				/** @} */
				header_type					write_header_;
				/** message deque
				*/
				std::deque< sp::shared_ptr<omessage> >		write_msgs_;
				/** process body
				*/
				/** thread write
				*/
				std::thread					write_thread_;
				/** thread read
				*/
				std::thread					read_thread_;
				/** condition variable
				*/
				std::condition_variable				cv_ready_;
				std::mutex					mutex_start_;
		};
	}
}




#endif





