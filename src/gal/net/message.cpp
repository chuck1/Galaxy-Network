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
	iar_ = new iarchive(ss_);
}
void			THIS::init_output()
{
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







