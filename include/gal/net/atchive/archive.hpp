#ifndef GAL_NET_ARCHIVE_ARCHIVE_HPP
#define GAL_NET_ARCHIVE_ARCHIVE_HPP

#include <gal/archive/archive.hpp>

#include <gal/net/decl.hpp>

namespace gal { namespace net { namespace archive {
	class archive:
		public gal::archive::archive
	{
	public:
		typedef gal::net::communicating C;
		std::shared_ptr<C>	get_return_address();
		std::weak_ptr<C>	_M_return_address;
	};
}}}

#endif

