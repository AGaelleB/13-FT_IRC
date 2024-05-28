#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"

class Server {

	private :
	
	public :
		Server();
		Server & operator=(Server const & rhs);
		Server(Server const & obj);
		~Server();

		struct sockaddr_in server_addr;



};

#endif // SERVER_HPP
