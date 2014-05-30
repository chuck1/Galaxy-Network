#ifndef __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__
#define __JESS_ASIO_NETWORK_SOCKET_COMMUNICATING_HPP__

#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <thread>
#include <condition_variable>

#include <sys/socket.h>

#include <Galaxy-Network/Types.hpp>
#include <Galaxy-Network/basic.hpp>
#include <Galaxy-Network/message.hpp>

namespace gal {
	namespace net {
		/** %socket %communicating
		 */
		class communicating: public gal::net::__basic {
			public:
				typedef int				header_type;
				enum { MAX_MESSAGE_LENGTH = 10000 };
			public:
				/** @brief ctor
				 *
				 * @param socket socket
				 */
				communicating(int socket);
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
				/** @brief thread write
				 *
				 * launch the read and write threads
				*/			
				void					start();
			private:
				communicating(communicating const &) = default;
				communicating(communicating &&) = default;
				communicating&				operator=(communicating const &) = default;
				communicating&				operator=(communicating &&) = default;
			protected:
				virtual void				process(sp::shared_ptr<gal::net::imessage>) = 0;
			private:
				/** thread write
				*/
				void					thread_write(sp::shared_ptr<gal::net::omessage>);
				/** @brief thread write dispath
				*/
				void					thread_write_dispatch();
				/** @brief thread read
				*/
				void					thread_read();
				/** @brief thread read header
				*/
				void					thread_read_header();
				/** @brief thread read body
				*/
				void					thread_read_body();
				/** @brief handle read header
				*/
				void					handle_do_read_header();
				/** @brief handle read body
				*/
				void					handle_do_read_body();
				/** @brief handle write
				*/
				void					handle_do_write();
			private:
				void					notify_bits(unsigned int bits);
			protected:			
				/** socket
				*/
				int					socket_;
			private:
				/** @name Read Data Members @{ */
				sp::shared_ptr<gal::net::imessage>		read_msg_;
				header_type					read_header_;
				char						read_buffer_[MAX_MESSAGE_LENGTH];
				/** @} */
				header_type					write_header_;
				/** message deque
				*/
				std::deque< sp::shared_ptr<omessage> >		write_queue_;
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





