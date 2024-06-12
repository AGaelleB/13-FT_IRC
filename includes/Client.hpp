#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./User.hpp"
#include "./Server.hpp"

class Server;

const int MIN_NAME_SIZE = 1; // A MODIFIER
const int MAX_NAME_SIZE = 15;

class Client {

	private:
		int					_client_socket;
		struct sockaddr_in	_client_addr;
		User				_user;

	public:
		Client();
		~Client();

		// Getters
		struct sockaddr_in&	getClientAddr();
		int					getClientSocket() const;
		User&				getUser();

		// Setters
		void				setClientSocket(int socket);
		void				setClientAddr(const struct sockaddr_in& addr);
		void				setUser(const User& user);

		// Functions
		void				sendClientMsg(int client_socket, const char* message);
		bool				checkName(const std::string& username);
		std::string			setUserName();
		std::string			setNickName(Server& server);

};

std::string	trim(const std::string& str);

#endif // CLIENT_HPP
