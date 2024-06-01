#ifndef SERVER_HPP
#define SERVER_HPP

#include "./Client.hpp"

enum AuthState {
    WAITING_FOR_PASSWORD,
    WAITING_FOR_USERNAME,
    WAITING_FOR_NICKNAME,
    AUTHENTICATED
};

struct ClientState {
    Client client;
    AuthState state;
    std::string username;
    std::string nickname;
};

class Server {

	private:
		int							_server_socket;
		struct sockaddr_in			_server_addr;
		std::string					_password;
		std::map<int, Client>		_clients;
		std::map<int, ClientState>	_clientStates;
		std::vector<struct pollfd>	_fds;
		int							_port;

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Functions
		void	startServer();
		void	addUser(Client &client, const std::string &username, const std::string &nickname);
		void	manage_new_client();
		bool	authenticateClient(Client &client);
		void	manage_new_data(int client_socket);
		void	remove_fd(int client_socket);
		void	close_fds();
		void	acceptClient();
		void	registrationClient(Client	client, int client_socket);
		void	handleClientMessage(int client_socket);
		void	removeClient(int client_socket);
};

#endif // SERVER_HPP
