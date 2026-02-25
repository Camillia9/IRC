#include "Replies.hpp"



// void	executeCommand(Server &server, Client &client, const t_command &cmd)
// {
// 	static std::map<std::string, CommandHandler> listeCommandes;

// 	if (listeCommandes)
// 	{

// 	}

// }






void execPass(Client *client, const t_command &cmd, const std::string &serverPasswd)
{
	if (client->isAuthenticated())
		IRC::errAlreadyReg(client);
	else if (cmd.params.empty())
		IRC::errMoreParams(client, cmd);
	else if (cmd.params[0] != serverPasswd)
		IRC::errPassMismatch(client);
	else
		client->setAuthenticated(true);
}

bool isSpecialChar(char c) {
    return std::string("[]{}|\\").find(c) != std::string::npos;
}

bool isValidNick(const std::string &str) {
    if (!isalpha(str[0]) && !isSpecialChar(str[0]))
        return false;
    for (size_t i = 1; i < str.size(); i++)
        if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-')
            return false;
    return true;
}

void execNick(Client *client, const t_command &cmd, const std::vector<Client*> &clients)
{
    if (cmd.params.empty())
        IRC::errNoNickGiven(client);
    else if (!isValidNick(cmd.params[0]) || cmd.params[0].size() > 9)
        IRC::errNickChar(client);
    else {
        for (size_t i = 0; i < clients.size(); i++) {
            if (cmd.params[0] == clients[i]->getNickname() && clients[i] != client) {
                IRC::errNickInuse(client, cmd.params[0]);
                return;
            }
        }
        client->setNickname(cmd.params[0]);
    }
}

void execUser(Client *client, const t_command &cmd)
{
	if (!client->isAuthenticated())
		return;
	else if (cmd.params.size() < 4)
		IRC::errMoreParams(client, cmd);
	else if (client->isRegistered())
		IRC::errAlreadyReg(client);
	else {
		client->setRegistered(true);
		client->setUsername(cmd.params[0]);
		client->setRealname(cmd.params[3]);
		IRC::sendWelcome(client);
	}	
}

//void execJoin(Client *client, const t_command &cmd, Server *server)
//{
//	if (!client->isRegistered())
//		return;
//	else if (cmd.params.empty()) {
//		IRC::errMoreParams(client, cmd);
//		return;
//	}

//	std::string channelName = cmd.params[0];

//	if (channelName[0] != '#') {
//		IRC::errNoSuchChannel(client, channelName);
//		return;
//	}
//	// 1. Verifier le format : (au moins un char apres '#', alphanumeric, '_', '-') 
//		// Si invalide errNoSuchChanel
	
//	// 2. Verifier si le channel existe, si non il se cree automatiquement

//	// 3. Ajouter le client au channel
//			// Le client doit etre dans la liste des membres du channel
//			// Le channel doit etre dans la liste des channels du client

//	// 4. Notifier les autres membres du channel et lui-meme
//		// ":nick!user@host JOIN #channel"

//	// 5. Envoyer le topic s'il existe (code 332/331)

//	// 6. Envoyer la liste des membres (le premier avec un '@') rpl 353,
//		// Puis envoyer Le EOF rpl 366
//}

//void execPart(Client *client, const t_command &cmd, Server *server)
//{
//	if (!client->isRegistered())
//		return;
//	else if (cmd.params.empty()) {
//		IRC::errMoreParams(client, cmd);
//		return;
//	}

//	std::string channelName = cmd.params[0];

//	if (channelName[0] != '#') {
//		IRC::errNoSuchChannel(client, channelName);
//		return;
//	}
//	// 1. Verifier que le channel existe

//	// 1.5. Verifier que le client est bien dans le channel. Si non rpl 442

//	// 2. Envoyer un message a tous les membres du channel (y compris celui qui part) :
//		// ":nick!user@host PART #channel"
//			// Si le client envoie message optionnel : PART #channel :bye
//			// Alors le message recu : ":nick!user@host PART #channel :bye"

//	// 3. Retirer le client de channel->_members et de client->_channels

//	// 4. Si vide, supprimer le channesls

	
//}


//void execPrvMsg(Client *client, const t_command &cmd, Server *server)
//{
//	if (!client->isRegistered())
//		return;
//	else if (cmd.params.empty()) {
//		IRC::errMoreParams(client, cmd);
//		return;
//	}
	
//}