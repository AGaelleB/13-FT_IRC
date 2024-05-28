#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"

class Server {

	private :
		struct sockaddr_in server_addr;
	
	public :
		Server();
		Server & operator=(Server const & rhs);
		Server(Server const & obj);
		~Server();




};

#endif // SERVER_HPP
