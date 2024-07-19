#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <unordered_set>
# include <vector>
# include <string>
# include "./Server.hpp"
# include "./Topic.hpp"

const int MIN_NAME_CHANNEL_SIZE = 1;
const int MAX_NAME_CHANNEL_SIZE = 32;

class Client;

class Channel {
	private:
		std::string						_channelName;
		std::vector<int>				_memberSockets; // Liste des sockets des membres du canal
		std::vector<int>				_membersOperators; // Liste des sockets des membres operateurs du canal
		std::vector<int>				_membersIsInvite;
		Topic							_topic;
		std::string						_mode;
		std::string						_password;
		bool							_topicRight;
		int								_maxMembersChannel;
		bool							_invitationAccess;
		std::unordered_set<std::string>	_bannedWords; // Liste des mots interdits
		void							initializeBannedWords();

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
		const std::vector<int>&	getMembersInvite() const;
		bool					getInvitationAccess() const;

		// setters
		void					setMode(const std::string& mode);
		void					setTopicRight(bool topicRight);
		void					setChannelKey(std::string password);
		void					setMaxMembers(int newMaxMember);
		void					setInvitationAccess(bool invitationAccess);

		// channel.cpp
		void					addMember(int clientSocket);
		void					removeMember(int clientSocket);
		bool					isMember(int clientSocket) const;

		// mode_o.cpp
		void					addOperator(int clientToAdd);
		void					removeOperator(int clientToErase);
		bool					isOperator(int clientSocket) const;

		// invite.cpp
		void					addInvitedMember(int clientToAdd);
		void					removeInvitedMember(int clientToErase);
		bool					isInvitedMember(int clientSocket) const;

		// banned_words.cpp
		void					addBannedWord(const std::string& word);
		void					removeBannedWord(const std::string& word);
		bool					isBannedWord(const std::string& word) const;

};

#endif //CHANNEL_HPP
