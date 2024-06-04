
// cmd irssi

// cmd IRC








// void Client::parsingCmd(const std::string& message, Client& client) {
// 	std::cout << BOLD << "\n" << client.getUser().getUsername()  <<  " msg: " << RESET << message;

// 	if (message.substr(0, 6) == "/login")
// 		std::cout << "Login command received" << std::endl;
// 	// else if (message.substr(0, 8) == "/channel")
// 	// 	std::cout << "Channel command received" << std::endl;
// 	else {
// 		// const char* unknownCommand = "Unknown command ❌\n";
// 		sendClientMsg(this->getClientSocket(), unknownCommand);
// 	}
// 	(void)client;
// }