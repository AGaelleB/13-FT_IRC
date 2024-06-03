#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./User.hpp"

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

		// Fonctions
		void				handleClientMsg(const std::string& message, Client& client);
		void				sendClientMsg(int client_socket, const char* message);
		void				welcomeClient(int client_socket);
};

#endif // CLIENT_HPP
