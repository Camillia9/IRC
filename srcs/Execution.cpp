#include "Execution.hpp"

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

void execNick(Client *client, const t_command &cmd, const std::vector<Client*> &clients)
{
	if (!client->isAuthenticated())
		return;
    else if (cmd.params.empty())
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
	if (client->getNickname().empty())
		return;
	else if (cmd.params.size() != 4)
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

void execJoin(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	if (cmd.params.empty()) {
		IRC::errMoreParams(client, cmd);
		return;
	}

	std::string channelName = cmd.params[0];
	if (!isValidChannelName(channelName)) {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}
	// 2. Verifier si le channel existe, si non il se cree automatiquement :
	bool isNewChannel = !server->doesChannelExist(channelName);
	Channel *channel = server->getOrCreateChannel(channelName);

	// 3. Ajouter le client au channel
	if (channel->isMember(client->getNickname()))
		return;
	else
	// AJOUTER LES MODES PLUS TARD : 
//1. Vérifier si déjà membre → return si oui
//2. Si PAS nouveau channel → vérifier les modes +i, +k, +l
//3. Si tout OK → addMember()
//4. Si nouveau channel → addOperator()
		channel->addMember(client);

	if (isNewChannel)
		channel->addOperator(client);
	// 4. Notifier les autres membres du channel et lui-meme
	std::string JOINmsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN " + channelName + "\r\n";
	channel->broadcast(JOINmsg);

	// 5. Envoyer le topic s'il existe (code 332/331)
	if (channel->getTopic().empty())
		IRC::rplNoTopic(client, channelName);
	else
		IRC::rplTopic(client, channelName, channel->getTopic());

	// 6. Envoyer la liste des membres (le premier avec un '@') rpl 353,
		// Puis envoyer Le EOF rpl 366

	std::string names = channel->getMembersList();
	IRC::rplNameReply(client, channelName, names);
	IRC::rplEndOfNames(client, channelName);
}

void execPart(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	else if (cmd.params.empty()) {
		IRC::errMoreParams(client, cmd);
		return;
	}
	std::string channelName = cmd.params[0];

	if (channelName[0] != '#') {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}
	// 1. Verifier que le channel existe
	Channel *channel = server->getChannel(channelName);
	if (!channel) {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}

	// 1.5. Verifier que le client est bien dans le channel. Si non rpl 442
	if (!channel->isMember(client->getNickname())) {
		IRC::errNotOnChannel(client, channelName);
		return;
	}

	// 2. Envoyer un message a tous les membres du channel (y compris celui qui part) : (avec ou sans raison en +)
		// ":nick!user@host PART #channel"
			// Si le client envoie message optionnel : PART #channel :bye
			// Alors le message recu : ":nick!user@host PART #channel :bye"
	std::string PartMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PART " + channelName;
	if (cmd.params.size() > 1)
		PartMsg += " :" + cmd.params[1];
	PartMsg += "\r\n";

	// Brodcaster le PART a tout les membres (y compris celui qui part)
	channel->broadcast(PartMsg);

	server->handleClientLeavingChannel(client, channel, channelName);
	
}


void execPrvMsg(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	if (cmd.params.empty()) {
		IRC::errNoRecipient(client, cmd);
		return;
	}
	if (cmd.params.size() < 2) {
		IRC::errNoTextSend(client);
		return;
	}
	std::string target = cmd.params[0]; // Destinataire
	std::string message = cmd.params[1]; 
	
	std::string fullMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";

	if (target[0] == '#') {
		// Boucler sur les channels existant
		// Verifier si existant
		// ERR_NOSUCHANEL
		Channel *channel = server->getChannel(target);
		if (!channel) {
			IRC::errNoSuchChannel(client, target);
			return;
		}
		// Boucler sur les utilsateurs du channels
		// Verifier que l'utilisateurs est membres
		if (!channel->isMember(client->getNickname())) {
			IRC::errCannotSendToChan(client, target);
			return;
		}
		// Envoyer le message a tous les embres sauf a lui-meme
		channel->broadcast(fullMsg, client);	
	}
	else {
		Client *recipient = server->getClientByNick(target);
		if (!recipient) {
			IRC::errNoSuchNick(client, target);
			return;
		}
		send(recipient->getFd(), fullMsg.c_str(), fullMsg.size(), 0);
	}
}

void execTopic(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	if (cmd.params.empty()) {
		IRC::errMoreParams(client, cmd);
		return;
	}
	std::string channelName = cmd.params[0];
	Channel *channel = server->getChannel(channelName);
	if (!channel) {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}
	if (!channel->isMember(client->getNickname())) {
		IRC::errNotOnChannel(client, channelName);
		return;
	}
	// Affichage du Topic
	if (cmd.params.size() == 1) {
		if (channel->getTopic().empty())
			IRC::rplNoTopic(client, channelName);
		else
			IRC::rplTopic(client, channelName, channel->getTopic());
		return;
	}
	// Changement du Topic
	if (cmd.params.size() > 1) {
		if (channel->isTopicRestricted() && !channel->isOperator(client->getNickname())) {
				IRC::errOpNeededToChanges(client, channelName);
				return;
		}
		//recupere le nouveau topic et oon le broadcaste (a tout le monde et lui-meme)
		channel->setTopic(cmd.params[1]);
		std::string Msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost TOPIC " + channelName + " :" + cmd.params[1] + "\r\n";
		channel->broadcast(Msg); 
	}
}

void execModes(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	if (cmd.params.empty()) {
		IRC::errMoreParams(client, cmd);
		return;
	}

	std::string channelName = cmd.params[0];
	Channel *channel = server->getChannel(channelName);

	if (!channel) {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}
	if (!channel->isMember(client->getNickname())) {
		IRC::errNotOnChannel(client, channelName);
		return;
	}
	if (cmd.params.size() == 1) {
		std::string modes = channel->getModesString();
		IRC::rplChannelMode(client, channelName, modes);
		return;
	}
	if (!channel->isOperator(client->getNickname())) {
		IRC::errOpNeededToChanges(client, channelName);
		return;
	}
	// Parser + Appliquer modes
}