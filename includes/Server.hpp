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
		std::set<std::string>	_nicknames;
		int						_port;
		std::string				_irssi_data;
		static bool				shutdown_signal;

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		// Server.cpp
		void    	sendDisconnectMessageToClients();
		void		stopServer();
		void		startServer();

		//AllCommands.cpp
		void		parseClientMsg(const std::string& message, Client& client);

		//help.cpp
		void		helpCmdClient(Client& client);
		void		helpCmdServer();

		//list.cpp
		void		witchList(std::string tokens, Client& client);
		void		channelList(Client& client);
		void		UserList(Client& client);

		//nick.cpp
		bool		isNicknameAvailable(const std::string& nickname); 
		void		removeNickname(const std::string& nickname);
		void		addNickname(const std::string& nickname);
		void		nickCmdClient(std::vector<std::string> tokens, Client& client);

		//pass.cpp
		bool		checkPasswordirssi(std::string answer, Client& client);

		// Authentification.cpp
		void		checkPassword(Client &client);
		void		addUser(Client &client, const std::string &username, const std::string &nickname);
		void		isRegistered(Client &client);
		void		authenticateAndRegister(Client &client);

		// parsing.cpp
		void		handleClientMessage(int client_fd, Client& client);
		void		logRPLirssi(Client& client);
		void		parsingDataIrssi(Client &client, int new_client_socket);
		void		parsingDataNetcat(Client &client, int new_client_socket);
		void		setNonBlocking(int socket);
		void		detectClient(int client_socket);

		// irsii.cpp
		int			findCapLs(const std::string& data);
		
		// Utils.cpp
		static void	SignalHandler(int sig);



};

#endif // SERVER_HPP
