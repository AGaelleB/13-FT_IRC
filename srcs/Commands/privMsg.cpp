#include "../../includes/Server.hpp"

void Server::privMsgCmdClient(Client& client, const std::string& target, const std::string& message) {
    int recipientSocket = -1;
    // Trouver le client destinataire par son surnom
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getUser().getNickname() == target) {
            recipientSocket = it->second.getClientSocket();
            break;
        }
    }

    // Si le destinataire n'est pas trouvé, envoyer une erreur au client expéditeur
    if (recipientSocket == -1) {
        client.sendClientMsg(client.getClientSocket(), "ERROR: Recipient not found");
        return;
    }

    // Construire le message à envoyer
    std::string fullMessage = "PRIVMSG from " + client.getUser().getNickname() + ": " + message + "\n";

    // Envoyer le message au client destinataire
    client.sendClientMsg(recipientSocket, fullMessage.c_str());
}