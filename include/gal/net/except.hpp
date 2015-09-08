#ifndef GAL_NET_EXCEPT_HPP
#define GAL_NET_EXCEPT_HPP

#include <gal/error/base.hpp>

namespace gal { namespace net { namespace except {

class ConnectFailed: public gal::error::base
{
public:

	ConnectFailed(gal::error::backtrace const & bt):
		gal::error::base(bt)
	{
	}
	virtual const char * what() const noexcept
	{
		_M_bt.print();
		return "item not found";
	}

};

}}}

#endif

