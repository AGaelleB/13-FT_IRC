#include "../includes/Server.hpp"

int Server::findCapLs(std::string s1) 
{
	std::string cap_ls = "CAP LS";
	
	std::cout << YELLOW << "Hi CAP LS" << RESET << std::endl;

	for (int i = 0; i < 6; ++i) {
		if (s1[i] != cap_ls[i])
			return 1;
	}
	return 0;
}

// int Server::check_irssi_entrance(int fd)
// {
// 	char buffer[1024];
// 	sleep(1);
// 	ssize_t bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);
	
// 	std::cout << YELLOW << "bytes_received check_irssi_entrance = " << bytes_received << RESET << std::endl;

// 	if (bytes_received > 0) 
// 	{
// 		buffer[bytes_received] = '\0';
// 		std::string answer(buffer);
// 		if (findCapLs(answer) == 0) {
// 			std::cerr << "connected with irssi!\n\n";
// 			this->_irssi_data = answer;
// 			return (1);
// 		}
// 	}
// 	else
// 		std::cerr << "connected with netcat!\n\n";
// 	return (0);
// }