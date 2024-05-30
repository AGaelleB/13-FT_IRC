#ifndef SERVER_HPP
#define SERVER_HPP

#include "./Client.hpp"

class Server {

	private:
		int							_server_socket;
		struct sockaddr_in			_server_addr;
		std::string					_password;
		std::map<int, Client>		_clients;
		std::vector<struct pollfd>	_fds;
		int							_port;

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Functions
		void	startServer();
		void addUser(Client &client, const std::string &username, const std::string &nickname);
		void manage_new_client();
		bool authenticateClient(Client &client);
		void manage_new_data(int client_socket);
		void remove_fd(int client_socket);
		void close_fds();
};

#endif // SERVER_HPP
