// int cmp(std::string s1) 
// {
// 	std::string cap_ls = "CAP LS";

// 	for (int i = 0; i < 6; ++i) {
// 		if (s1[i] != cap_ls[i])
// 			return 1;
// 	}
// 	return 0;
// }

// int Server::check_irssi_entrance(int fd)
// {
// 	char buff[1024];
// 	sleep(1);
// 	int byte = recv(fd, buff, sizeof(buff), MSG_DONTWAIT);
// 	if (byte > 0) 
// 	{
// 		buff[byte] = '\0';
// 		std::string answer(buff);
// 		if (cmp(answer) == 0) {
// 			std::cerr << "!irssi connexion!\n\n";
// 			this->irssi_base = answer;
// 			return 1;
// 		}
// 	}
// 	else
// 		std::cerr << "!netcat connexion!\n\n";
// 	return 0;
// }