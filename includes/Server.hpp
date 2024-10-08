// Server.hpp

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Client.hpp"
# include "./Channel.hpp"

# define MAX_SIZE_MSG 350

extern struct pollfd fds[1024];
extern int nfds;
extern int poll_count;

class Client;
class Channel;

enum CommandType {
	HELP,
	NICK,
	LIST,
	PRIVMSG,
	CHANNEL,
	QUIT,
	PART,
	TOPIC,
	KICK,
	INVITE,
	PING,
	MODE,
	UNKNOWN
};

enum ModeType {
	MODE_I,
	MODE_T,
	MODE_K,
	MODE_O,
	MODE_L,
	MODE_B,
	MODE_UNKNOWN
};

class Server {
	private:
		static const int				_MAX_CLIENTS = 20;
		int								_server_socket;
		struct sockaddr_in				_server_addr;
		std::string						_password;
		std::map<int, Client>			_clients;
		std::set<std::string>			_nicknames;
		int								_port;
		std::string						_irssi_data;
		static bool						_shutdown_signal;	
		std::map<std::string, Channel>	_channels;
		std::vector<std::string>		_channelOrder;


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
		Client&		getClientBySocket(int socket);

		/******************************* COMMANDES ********************************/

		// AllCommands.cpp
		CommandType	getCommandType(const std::string& command);
		void		parseClientMsg(const std::string& message, Client& client);
		void		handleUnknownCommand(Client& client, const std::vector<std::string>& tokens);

		// bot.cpp
		void		addBannedWordCmd(Client& client, const std::vector<std::string>& tokens, Channel& channel);
		bool		checkBannedWordsPrivMsg(Client& client, Channel& channel, const std::string& channelName, const std::string& msgContent);
		bool		checkBannedWordsUnknown(Client& client, Channel& channel, const std::string& channelName, const std::string& msgContent);

		// channel.cpp
		bool		checkChannelName(const std::string& channelName);
		void		createChannel(Client& client, std::string channelName);
		
		// join.cpp
		bool		validateTokensJoin(Client& client, const std::vector<std::string>& tokens);
		void		handleChannel(Client& client, std::string& channelName, const std::vector<std::string>& tokens);
		void		joinCmdClient(Client& client, const std::vector<std::string>& tokens);
		void		sendChannelJoinInfo(Channel& channel, const std::string& channelName, Client& client);
		
		// channelMsg.cpp
		void		broadcastMessageToChannel(const std::string& channelName, const std::string& message, int excludeSocket);
		void		ChannelMsg(Client& client);
		
		// help.cpp
		void		helpCmdClient(Client& client);

		// invite.cpp
		void		sendErrorMessage(Client& client, const std::string& errorMsgNetcat, const std::string& errorMsgIrssi);
		int			findUserSocket(const std::string& nickname);
		bool		validateInviteCommand(Client& client, const std::vector<std::string>& tokens);
		bool		validateChannelMembership(Client& client, const std::string& channelName);
		void		sendInviteMessages(Client& client, Channel& channel, const std::string& channelName, Client& invitedClient, int userSocket);
		void		inviteCmdClient(Client& client, std::vector<std::string> tokens);

		// leave.cpp
		void		partAllChannels(Client& client);
		void		partCmdClientIRSSI(Client& client, std::vector<std::string> tokens);
		void		partCmdClientNC(Client& client, std::vector<std::string> tokens);
		void		partCmdClientCommon(Client& client, const std::string& channelName, const std::string& reason);
		void		partCmdClient(Client& client, std::vector<std::string> tokens);
		std::string	joinTokens(const std::vector<std::string>& tokens, size_t startIndex);

		// list.cpp
		void		listCmdClient(std::vector<std::string> tokens, Client& client);
		void		UserList(Client& client);
		void		channelList(Client& client);
		void		channelListMembers(int clientSocket, const std::string& channelName, Client& client);
		std::string	PrintChannelListMembers(const std::string& channelName, const std::map<std::string, Channel>& channels);

		// mode.cpp
		ModeType	getModeType(const std::string& modeType);
		bool		validateTokensMode(Client& client, const std::vector<std::string>& tokens);
		void		modeCmdClient(Client& client, std::vector<std::string> tokens);
		

		// mode_i.cpp
		void		modeBCmd(Client& client, const std::vector<std::string>& tokens, Channel& channel, const std::string& channelName);

		// mode_i.cpp
		void		modeICmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);

		// mode_k.cpp
		void		modeKCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);

		// mode_l.cpp
		void		modeLCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);

		// mode_o.cpp
		void		modeOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);
		void		modePlusOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);
		void		modeMinusdOCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);
		
		// mode_t.cpp
		void		modeTCmd(Client& client, std::vector<std::string> tokens, Channel& channel, std::string channelName);

		// nick.cpp
		bool		isNicknameAvailable(const std::string& nickname); 
		void		removeNickname(const std::string& nickname);
		void		addNickname(const std::string& nickname);
		void		nickCmdClient(std::vector<std::string> tokens, Client& client);

		// pass.cpp
		bool		checkPasswordirssi(std::string answer, Client& client);

		// ping.cpp
		void		pingCmdClient(std::vector<std::string> tokens, Client& client);

		// privMsg.cpp
		bool		validateTokensPrivMsg(Client& client, const std::vector<std::string>& tokens);
		std::string	extractMessageContent(const std::string& message, const std::string& targets);
		void		sendMessageToChannel(Server& server, Client& client, const std::string& target, const std::string& msgContent);
		void		sendMessageToUser(Server& server, Client& client, const std::string& target, const std::string& msgContent);
		void		handleTarget(Server& server, Client& client, const std::string& target, const std::string& msgContent);
		void		privMsgCmdClient(Client& client, const std::vector<std::string>& tokens, const std::string& message);

		// quit.cpp
		void		quitCmdClientReason(Client& client, std::vector<std::string> tokens, std::string old_nickname);
		void		quitCmdClient(Client& client, std::vector<std::string> tokens);

		// topic.cpp
		bool		validateTokensTopic(Client& client, const std::vector<std::string>& tokens);
		void		topicInfo(Client& client, Channel& channel, std::string channelName);
		void		topicSetUp(Client& client, Channel& channel, std::string channelName, std::vector<std::string> tokens);
		void		topicCmdClient(Client& client, std::vector<std::string> tokens);

		// kick.cpp
		void		kickCmdClientReason(Client& client, Channel& channel, std::vector<std::string> tokens, int userSocket);
		void		executeKick(Client& client, Channel& channel, const std::string& reason, int userSocket);
		void		kickCmdClient(Client& client, std::vector<std::string> tokens);


		/******************************* REGISTRATION *******************************/

		// Authentification.cpp
		void		checkPassword(Client &client);
		void		addUser(Client &client, const std::string &username, const std::string &nickname);
		bool		isRegistered(Client &client);
		void		handleClientDisconnection(int client_fd);
		void		removeClientFromPoll(int client_fd);
		void		authenticateAndRegister(Client &client);

		// parsing.cpp
		void		handleClientMessage(int client_fd, Client& client);
		void		logRPLirssi(Client& client);
		void		parsingDataIrssi(Client &client, int new_client_socket);
		bool		parsingDataNetcat(Client &client, int new_client_socket);
		void		setNonBlocking(int socket);
		void		detectClient(int client_socket);

		// irsii.cpp
		int			findCapLs(const std::string& data);
		
		// Utils.cpp
		static void	SignalHandler(int sig);
		static void TstpSignalHandler(int sig);
};

#endif // SERVER_HPP