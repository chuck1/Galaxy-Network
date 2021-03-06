#ifndef __JESS_ASIO_MESSAGE_HPP__
#define __JESS_ASIO_MESSAGE_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <memory>
#include <deque>

#include <gal/shared.hpp>
#include <gal/verb/Verbosity.hpp>

//#if defined GAL_NET_USE_BOOST
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>

#include <gal/archive/polymorphic_binary_oarchive.hpp>
#include <gal/archive/polymorphic_binary_iarchive.hpp>

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
		virtual public gal::verb::Verbosity<gal::net::message>,
		virtual public gal::enable_shared_from_this<message>
	{
		public:
			friend class gal::net::communicating;
			using gal::verb::Verbosity<gal::net::message>::printv;
			//typedef boost::archive::polymorphic_binary_oarchive oarchive;
			//typedef boost::archive::polymorphic_binary_iarchive iarchive;
			typedef gal::archive::polymorphic_binary_oarchive oarchive;
			typedef gal::archive::polymorphic_binary_iarchive iarchive;
			typedef std::shared_ptr<oarchive> S_OA;
			typedef std::shared_ptr<iarchive> S_IA;

			/// ctor
			message();
			virtual ~message();
			virtual void			init_input(gal::mng::managed_object *);
			virtual void			init_output(gal::mng::managed_object *);
			void				set_iar_factory_map(
					std::shared_ptr<gal::stl::factory_map> fm);
			void				set_oar_factory_map(
					std::shared_ptr<gal::stl::factory_map> fm);
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
			virtual void			reset_iarchive() = 0;
			virtual void			reset_oarchive() = 0;
			S_OA				get_oar();
			S_IA				get_iar();
		protected:
			std::stringstream		ss_;
			S_OA				oar_;
			S_IA				iar_;
	};
}}

#endif

