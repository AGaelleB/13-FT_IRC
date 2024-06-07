#include "../../includes/Server.hpp"

int Server::findCapLs(const std::string& data) 
{
	std::string cap_ls = "CAP LS";
	for (int i = 0; i < 6; ++i) {
		if (data[i] != cap_ls[i])
			return 1;
	}
	return 0;
}

