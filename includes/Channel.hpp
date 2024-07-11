#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "./Server.hpp"
# include "./Topic.hpp"

const int MIN_NAME_CHANNEL_SIZE = 1;
const int MAX_NAME_CHANNEL_SIZE = 32;

class Channel {
	private:
		std::string				_channelName;
		std::vector<int>		_memberSockets; // Liste des sockets des membres du canal
		Topic					_topic;
		std::string				_mode;

	public:
		Channel();
		Channel(const std::string& channelName);
		~Channel();

		const std::string&		getName() const;
		const std::vector<int>&	getMembers() const;
		int						getMembersCount() const;
		const Topic&			getTopic() const;
		Topic&					getTopic();
		std::string	getMode() const;

		void		setMode(const std::string& mode);

		void					addMember(int clientSocket);
		void					removeMember(int clientSocket);
		bool					isMember(int clientSocket) const;


};
#endif //CHANNEL_HPP