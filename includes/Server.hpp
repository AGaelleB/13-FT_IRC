#ifndef SERVER_HPP
#define SERVER_HPP

#include "./Client.hpp"

extern struct pollfd fds[1024];
extern int nfds;

class Client;

class Server {

	private:
		int						_server_socket;
		struct sockaddr_in		_server_addr;
		std::string				_password;
		std::map<int, Client>	_clients;
        std::set<std::string>   _nicknames;
		int						_port;
		std::string				_irssi_data;


	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Server.cpp
		void	startServer();

		// Authentification.cpp
		bool    isNicknameAvailable(const std::string& nickname); 
		void	handleClientMessage(int client_index, Client& client);
		void	checkPassword(Client &client);
		void	addUser(Client &client, const std::string &username, const std::string &nickname);
		void	authenticateAndRegister(Client &client);


		// irsii.cpp
		int		findCapLs(std::string s1);
		int		check_irssi_entrance(int fd);



};

#endif // SERVER_HPP
