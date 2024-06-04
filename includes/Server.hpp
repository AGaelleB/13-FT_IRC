#ifndef SERVER_HPP
#define SERVER_HPP

#include "./Client.hpp"

extern struct pollfd fds[1024];
extern int nfds;

class Client; // Déclaration anticipée de Client

class Server {

	private:
		int						_server_socket;
		struct sockaddr_in		_server_addr;
		std::string				_password;
		std::map<int, Client>	_clients;
        std::set<std::string>   _nicknames;
		int						_port;


	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Functions
		void	addUser(Client &client, const std::string &username, const std::string &nickname);
		void	handleClientMessage(int client_index, Client& client);
		void	startServer();
		void	checkPassword(Client &client);
		void	authenticateAndRegister(Client &client);
		bool    isNicknameAvailable(const std::string& nickname); 

};

std::string	trim(const std::string& str);

#endif // SERVER_HPP
