#include "../includes/ircserv.hpp"

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << RED << "Error: must be ./ircserv <port> <password>" << RESET << std::endl;
		return 1;
	}
	(void)av;


	return 0;
}
