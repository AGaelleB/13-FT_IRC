#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/Library.hpp"
#include "../includes/Client.hpp"

class Server {

	private:
		int					_server_socket;
		struct sockaddr_in	_server_addr;
		std::string			_password;

	public:
		Server();
		Server(int port, const std::string &password);
		~Server();

		// Functions
		void	startServer();
		int		port;
};

#endif // SERVER_HPP
