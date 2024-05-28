
#include "../includes/Client.hpp"


/****************************************** FORME CANONIQUE ******************************************/

Client::Client() {
}

Client & Client::operator=(Client const & rhs) {
	if (this == &rhs)
		return (*this);
	return (*this);
}

Client::Client(Client const & obj) {
	*this = obj;
}

Client::~Client() {
}

