
#include "../includes/ircserv.hpp"


/****************************************** FORME CANONIQUE ******************************************/

ircserv::ircserv() {
}

ircserv & ircserv::operator=(ircserv const & rhs) {
	if (this == &rhs)
		return (*this);
	return (*this);
}

ircserv::ircserv(ircserv const & obj) {
	*this = obj;
}

ircserv::~ircserv() {
}

