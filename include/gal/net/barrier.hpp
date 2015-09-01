#ifndef GAL_NET_BARRIER_HPP
#define GAL_NET_BARRIER_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace gal { namespace net {

/** \brief a simple barrier
 *
 * N - number of threads to wait for before releasing
 */
template<int N>
class Barrier
{
public:
	Barrier():
		_M_c(0)
	{}
	void	wait()
	{
		std::unique_lock<std::mutex> lk(_M_mutex);

		_M_c++;

		int c = _M_c;
		printf("barrier %p %i %i\n", this, N, c);

		if(_M_c == N) {
			_M_cv.notify_all();
			// prepare for next use
			_M_c = 0;
		} else {
			_M_cv.wait(lk);
		}
	}
	/** number of threads currently waiting
	 */
	std::atomic<int>	_M_c;

	std::mutex		_M_mutex;
	std::condition_variable	_M_cv;
};

}}

#endif



