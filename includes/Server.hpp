// Server.hpp

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Client.hpp"
# include "./Channel.hpp"

# define MAX_SIZE_MSG 150

extern struct pollfd fds[1024];
extern int nfds;

class Client;
class Channel;

class Server {
	private:
		static const int		_MAX_CLIENTS = 20;		
		int						_server_socket;
		struct sockaddr_in		_server_addr;
		std::string				_password;
		std::map<int, Client>	_clients;
		std::set<std::string>	_nicknames;
		int						_port;
		std::string				_irssi_data;
		static bool				_shutdown_signal;	
		std::map<std::string, Channel> _channels; // Liste des canaux

	public:
		Server();
		Server(int _port, const std::string &password);
		~Server();

		/********************************* SERVER **********************************/

		// Server.cpp
		void		initializeServer();
		void		acceptNewConnection();
		void		handlePollEvents();
		void		stopServer();
		void		startServer();


		/******************************* COMMANDES ********************************/

		//AllCommands.cpp
		void		parseClientMsg(const std::string& message, Client& client);

		// channel.cpp
		bool		checkChannelName(const std::string& channelName);
		void		createChannel(Client& client, const std::vector<std::string>& tokens);
		
		// join
		// void		joinChannel(int clientSocket, const std::string& channelName);

		// leave
		// void		leaveChannel(int clientSocket, const std::string& channelName);

		//help.cpp
		void		helpCmdClient(Client& client);
		void		helpCmdServer();

		//list.cpp
		void		listCmdClient(std::vector<std::string> tokens, Client& client);
		void		channelList(Client& client);  // A FAIRE
		void		channelListMembers(int clientSocket, const std::string& channelName); // A FAIRE
		void		UserList(Client& client);

		//nick.cpp
		bool		isNicknameAvailable(const std::string& nickname); 
		void		removeNickname(const std::string& nickname);
		void		addNickname(const std::string& nickname);
		void		nickCmdClient(std::vector<std::string> tokens, Client& client);

		//pass.cpp
		bool		checkPasswordirssi(std::string answer, Client& client);

		//ping.cpp
		void		pingCmdClient(std::vector<std::string> tokens, Client& client);

		//privMsg.cpp
		void		privMsgCmdClient(Client& client, const std::vector<std::string>& tokens, const std::string& message);

		//quit.cpp
		void		quitMsgCmdClient(Client& client, std::vector<std::string> tokens);



		/******************************* REGISTRATION *******************************/

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
