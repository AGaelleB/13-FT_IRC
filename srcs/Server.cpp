
#include "../includes/Server.hpp"


/****************************************** FORME CANONIQUE ******************************************/

Server::Server() {
}

Server & Server::operator=(Server const & rhs) {
	if (this == &rhs)
		return (*this);
	return (*this);
}

Server::Server(Server const & obj) {
	*this = obj;
}

Server::~Server() {
}

