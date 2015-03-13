#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>

//#include <galaxy/config.hpp>
#include <gal/net/message.hpp>

typedef gal::net::message THIS;

THIS::message():
	oar_(0),
	iar_(0)
{
}
THIS::~message()
{
}
/*void gal::net::message::set(void const * const v, unsigned int len) {
	//GALAXY_DEBUG_1_FUNCTION;
	
	assert(v);
	
	assert(len < Neb::Network::MAX_MESSAGE_LENGTH);
	
	memcpy(body(), v, len);
	
	body_length(len);
	encode_header();
}*/
void			THIS::reset_head()
{
	ss_.seekp(0);
}
/*void		gal::net::message::write(void const * const v, size_t len) {
	assert(v);
	
	ss_.write((char*)v,len);
}
void		gal::net::message::read(void * const v, size_t len) {
	assert(v);
	
	ss_.read((char*)v,len);
	
	if(ss_.fail()) abort();
	}*/
void			THIS::init_input()
{
	if(iar_) return;

	try {
		iar_ = new iarchive(ss_);
	} catch(boost::archive::archive_exception& e) {
		printf("error: %s\n", e.what());

		std::string s = ss_.str();
		printf("string length = %lu\n", s.length());

		unsigned char const * c = (unsigned char const *)s.c_str();
		
		printf("data:\n");
		for(unsigned int i = 0; i < s.length(); i++) {
			printf("%02X ", c[i]);
		}
		printf("\n");

		printf("pos: %i\n", (int)ss_.tellg());

		abort();
	}

}
void			THIS::init_output()
{
	if(oar_) return;

	oar_ = new oarchive(ss_);
}
THIS::oarchive*		THIS::get_oar()
{
	assert(oar_);
	return oar_;
}
THIS::iarchive*		THIS::get_iar()
{
	assert(iar_);
	return iar_;
}







