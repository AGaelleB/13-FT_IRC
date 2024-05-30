#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./Library.hpp"
#include "./User.hpp"

class Client {

	private:
		int					_client_socket;
		struct sockaddr_in	_client_addr;
		User				_users[1000];

	public:
		Client();
		~Client();

		// Getters
		int					getClientSocket() const;
		struct sockaddr_in&	getClientAddr();

		// Setters
		void				setClientSocket(int socket);
		void				setClientAddr(const struct sockaddr_in& addr);

		// Fonctions
		void				handleClientMsg(const std::string& message, Client& client);
		void				sendClientMsg(int client_socket, const char* message);
		void				welcomeClient(int client_socket);
};

#endif // CLIENT_HPP
