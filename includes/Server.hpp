#ifndef SERVER_HPP
#define SERVER_HPP

#include "./Client.hpp"

extern struct pollfd fds[1024];
extern int nfds;


class Server {

	private:
		int						_server_socket;
		struct sockaddr_in		_server_addr;
		std::string				_password;
		std::map<int, Client>	_clients;
		int						_port;

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Functions
		void	startServer();
		void addUser(Client &client, const std::string &username, const std::string &nickname);
		void handleClientMessage(int client_index);
		void authenticateAndRegister(Client &client);
};

#endif // SERVER_HPP
