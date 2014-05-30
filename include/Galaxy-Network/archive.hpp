#ifndef __JESS_ASIO_ARCHIVE_HPP
#define __JESS_ASIO_ARCHIVE_HPP

namespace gal {
	namespace net {
		class iarchive {
			public:
				iarchive(std::stringstream& ss): ss_(ss) {
				}
				std::stringstream&	ss_;
		};
		class oarchive {
			public:
				oarchive(std::stringstream& ss): ss_(ss) {
				}
				std::stringstream&	ss_;
		};
	}
}

#endif
