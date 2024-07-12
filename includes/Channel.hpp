#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "./Server.hpp"
# include "./Topic.hpp"

const int MIN_NAME_CHANNEL_SIZE = 1;
const int MAX_NAME_CHANNEL_SIZE = 32;

class Client;

class Channel {
	private:
		std::string				_channelName;
		std::vector<int>		_memberSockets; // Liste des sockets des membres du canal
		std::vector<int>		_membersOperators; // Liste des sockets des membres operateurs du canal
		Topic					_topic;
		std::string				_mode;
		std::string				_password;
		bool					_topicRight;
		int			_maxMembersChannel;

	public:
		Channel();
		Channel(const std::string& channelName);
		~Channel();

		// getters
		const std::string&		getName() const;
		const std::vector<int>&	getMembers() const;
		int						getMembersCount() const;
		const Topic&			getTopic() const;
		Topic&					getTopic();
		std::string				getMode() const;
		bool					getTopicRight() const;
		int						getMaxMembers() const;
		const std::string&		getChannelKey() const;
		const std::vector<int>&	getOperators() const;

		// setters
		void					setMode(const std::string& mode);
		void					setTopicRight(bool keypass_set);
		void					setChannelKey(std::string password);
		void					setMaxMembers(int newMaxMember);
		// channel.cpp
		void					addMember(int clientSocket);
		void					removeMember(int clientSocket);
		bool					isMember(int clientSocket) const;

		// mode_o.cpp
		
		void					addOperator(int clientToAdd);
		void					removeOperator(int clientToErase);
		bool					isOperator(int clientSocket) const;




};
#endif //CHANNEL_HPP