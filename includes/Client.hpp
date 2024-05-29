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

		int getClientSocket() const;
		struct sockaddr_in& getClientAddr();
		void setClientSocket(int socket);
		void setClientAddr(const struct sockaddr_in& addr);
};

#endif // CLIENT_HPP
