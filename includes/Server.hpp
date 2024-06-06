// Server.hpp

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
		static bool				shutdown_signal;

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Server.cpp
		void	startServer();
		void	stopServer(); // Nouvelle méthode pour arrêter le serveur
		void    sendDisconnectMessageToClients();

		// Authentification.cpp
		bool	isNicknameAvailable(const std::string& nickname); 
		void	handleClientMessage(int client_index, Client& client);
		void	checkPassword(Client &client);
		void	addUser(Client &client, const std::string &username, const std::string &nickname);
		void	isRegistered(Client &client);
		void	authenticateAndRegister(Client &client);

		// irsii.cpp
		int		findCapLs(const std::string& data);
		int		check_irssi_entrance(int fd);

		// parsing.cpp
		void	detectClient(int client_socket);
		void	parsingDataIrssi(Client &client, int new_client_socket);
		void	parsingDataNetclient(Client &client, int new_client_socket);
		void	setNonBlocking(int socket);

		static void	SignalHandler(int sig);

		// Utils.cpp
		void				removeNickname(const std::string& nickname);
		void				helpCmdServer();
		void				testRemoveNickname();
};

#endif // SERVER_HPP
