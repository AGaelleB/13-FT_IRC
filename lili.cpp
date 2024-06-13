void Server::handleClientMessage(int client_fd, Client& client) {
    char buffer[MAX_SIZE_MSG];
    int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        // Handle client disconnection
        close(client_fd);
        for (int i = 0; i < nfds; ++i) {
            if (fds[i].fd == client_fd) {
                fds[i].fd = -1;
                break;
            }
        }
        _clients.erase(client_fd);
    } else {
        buffer[bytes_read] = '\0';
        std::string message(buffer);

        // Check if client is part of a channel and broadcast the message
        for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
            Channel& channel = it->second;
            if (channel.isMember(client_fd)) {
                std::string fullMessage = "<" + client.getUser().getNickname() + "> " + message;
                broadcastMessageToChannel(it->first, fullMessage, client_fd);
                break;
            }
        }
    }
}