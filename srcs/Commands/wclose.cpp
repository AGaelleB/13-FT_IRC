#include "../../includes/Server.hpp"

void Server::wcloseCmdClient(Client& client) {
    std::string response;
    if (client.isIrssi) {
        response = ":localhost 461 " + client.getUser().getNickname() + " WCLOSE :Window closed\r\n";
    } else {
        response = "Window closed\n";
    }
    client.sendClientMsg(client.getClientSocket(), response.c_str());
}
