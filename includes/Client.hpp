#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./Library.hpp"

class Client {

	private:
		int					_client_socket;
		struct sockaddr_in	_client_addr;

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
		void				handleClientMessage(const std::string& message, Client& client);
		void				sendMsgClient(int client_socket, const char* message);
		void				welcomeClient(int client_socket);
};

#endif // CLIENT_HPP
