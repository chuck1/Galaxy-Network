#ifndef __JESS_ASIO_MESSAGE_HPP__
#define __JESS_ASIO_MESSAGE_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <memory>
#include <deque>


//#if defined GAL_NET_USE_BOOST
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>


/*
#else

#include <gal/net/archive.hpp>

typedef gal::net::oarchive oarchive;
typedef gal::net::iarchive iarchive;

#endif
*/

#include <gal/net/decl.hpp>

namespace gal { namespace net {
	/// message
	class message:
		public std::enable_shared_from_this<message>
	{
		public:
			friend class gal::net::communicating;
			typedef boost::archive::polymorphic_binary_oarchive oarchive;
			typedef boost::archive::polymorphic_binary_iarchive iarchive;
			/// ctor
			message();
			virtual ~message();
			void				init_input();
			void				init_output();
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
			oarchive*			get_oar();
			iarchive*			get_iar();
		protected:
			std::stringstream		ss_;
			oarchive*			oar_;
			iarchive*			iar_;
	};
}}

#endif

