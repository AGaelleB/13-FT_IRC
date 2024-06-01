

    switch (clientState.state) {
        case WAITING_FOR_PASSWORD:
            if (message == _password) {
                clientState.state = WAITING_FOR_USERNAME;
                clientState.client.sendClientMsg(client_socket, BOLD "Enter your username: " RESET);
            } else {
                clientState.client.sendClientMsg(client_socket, RED "Wrong password \n\n" RESET);
                clientState.client.sendClientMsg(client_socket, BOLD "Enter Server password: " RESET);
            }
            break;

        case WAITING_FOR_USERNAME:
            clientState.username = message;
            clientState.state = WAITING_FOR_NICKNAME;
            clientState.client.sendClientMsg(client_socket, BOLD "Enter your nickname: " RESET);
            break;

        case WAITING_FOR_NICKNAME:
            clientState.nickname = message;
            addUser(clientState.client, clientState.username, clientState.nickname);
            clientState.client.sendClientMsg(client_socket, GREEN "You are now registered! ​✅\n\n" RESET);
            clientState.state = AUTHENTICATED;
            break;

        case AUTHENTICATED:
            // Ici vous pouvez gérer les messages des clients authentifiés
            std::cout << "Client #" << client_socket << " is now authenticated" << std::endl;
            break;
    }
}