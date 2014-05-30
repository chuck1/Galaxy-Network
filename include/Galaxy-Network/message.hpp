#ifndef __JESS_ASIO_MESSAGE_HPP__
#define __JESS_ASIO_MESSAGE_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <memory>
#include <deque>

#if defined GALNET_USE_BOOST
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#else
#error
#endif

//namespace sp = std;

#include <Galaxy-Network/config.hpp>
#include <Galaxy-Network/Types.hpp>

namespace gal {
	namespace net {
		/// message
		class message: public sp::enable_shared_from_this<message> {
			public:
				friend class gal::net::communicating;
			public:
				/// ctor
				message();
				virtual ~message() = 0;
				//void				set(void const * const, unsigned int);
				void				reset_head();
				/*void				write(void const * const, size_t);
				template<typename T> void	write(const T& t) {
					write(&t, sizeof(T));
				}*/

				/*template<class T> message&	operator<<(T t) {
				  t.serialize(*this, 0);
				  return *this;
				  }*/


				/*void				read(void * const, size_t);
				template<typename T> void	read(T& t) {
					read(&t, sizeof(T));
				}*/
				
			public: //protected:
				std::stringstream		ss_;
		};
		class omessage: public message {
			public:
				omessage();
				boost::archive::polymorphic_binary_oarchive		ar_;
		};
		class imessage: public message {
			public:
				imessage();
				boost::archive::polymorphic_binary_iarchive		ar_;
		};
	}
}














#endif // __JESS_ASIO_MESSAGE_HPP__
