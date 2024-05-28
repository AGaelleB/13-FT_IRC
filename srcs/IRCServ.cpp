
#include "../includes/IRCServ.hpp"


/****************************************** FORME CANONIQUE ******************************************/

IRCServ::IRCServ() {
}

IRCServ & IRCServ::operator=(IRCServ const & rhs) {
	if (this == &rhs)
		return (*this);
	return (*this);
}

IRCServ::IRCServ(IRCServ const & obj) {
	*this = obj;
}

IRCServ::~IRCServ() {
}

