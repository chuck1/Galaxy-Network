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
#include <Galaxy-Network/message.hpp>

namespace gal {
	namespace net {
		/** %socket %communicating
		 */
		class communicating {
			public:
				typedef int				header_type;
				enum { MAX_MESSAGE_LENGTH = 10000 };
			public:
				/** ctor
				 */
				communicating(int socket);
				/** write
				 */
				void					write(sp::shared_ptr<omessage>);
				/** close
				 */
				void					close();
				//	protected:
				/** thread write
				 */
				void					start();
				virtual void				process(sp::shared_ptr<gal::net::imessage>) = 0;
				void					thread_write(sp::shared_ptr<gal::net::omessage>);
				/** thread write dispath
				 */
				void					thread_write_dispatch();
				/** thread read
				 */
				void					thread_read();
				/** thread read header
				 */
				void					thread_read_header();
				/** thread read body
				 */
				void					thread_read_body();
				/** handle read header
				 */
				void					handle_do_read_header();
				/** handle read body
				 */
				void					handle_do_read_body();
				/** handle write
				 */
				void					handle_do_write();
				//	protected:
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
				bool						terminate_;
				/** thread write
				 */
				std::thread					write_thread_;
				/** thread read
				 */
				std::thread					read_thread_;
				/** condition variable
				 */
				std::condition_variable				cv_;
				std::condition_variable				cv_ready_;
				/** mutex
				 * mutex for write_queue_ and terminate_
				 */
				std::mutex					mutex_;
				std::mutex					mutex_start_;
		};
	}
}




#endif





