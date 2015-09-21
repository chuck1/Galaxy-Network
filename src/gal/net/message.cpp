#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>

#include <gal/stl/factory_map.hpp>

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
	
	if(ss_.fail()) assert(0);
	}*/
void			THIS::init_input(gal::mng::managed_object * shared_parent)
{
	if(iar_) return;
	
	assert(shared_parent);

	auto s = shared_parent->shared_from_this();

	try {
		//iar_ = new iarchive(ss_);
		reset_iarchive();
		
		//iar_->_M_shared_parent = shared_parent;
		iar_->gal::mng::managed_object::init(shared_parent->get_registry());

	} catch(boost::archive::archive_exception& e) {
		printf("error: %s\n", e.what());

		std::string st = ss_.str();
		printf("string length = %lu\n", st.length());

		unsigned char const * c = (unsigned char const *)st.c_str();
		
		printf("data:\n");
		for(unsigned int i = 0; i < st.length(); i++) {
			printf("%02X ", c[i]);
		}
		printf("\n");

		printf("pos: %i\n", (int)ss_.tellg());

		assert(0);
	}
}
void			THIS::init_output(gal::mng::managed_object * shared_parent)
{
	if(oar_) return;

	assert(shared_parent);

	auto s = shared_parent->shared_from_this();

	//oar_ = new oarchive(ss_);
	reset_oarchive();
	
	//oar_->_M_shared_parent = shared_parent;
	oar_->gal::mng::managed_object::init(shared_parent->get_registry());
}
void			THIS::set_oar_factory_map(
		std::shared_ptr<gal::stl::factory_map> fm)
{
	assert(fm);
	oar_->_M_factory_map = fm;
}
void			THIS::set_iar_factory_map(
		std::shared_ptr<gal::stl::factory_map> fm)
{
	assert(fm);
	iar_->_M_factory_map = fm;
}
THIS::S_OA		THIS::get_oar()
{
	assert(oar_);
	return oar_;
}
THIS::S_IA		THIS::get_iar()
{
	assert(iar_);
	return iar_;
}







