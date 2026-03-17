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
		std::string oldNick = client->getNickname();
        client->setNickname(cmd.params[0]);

        // Broadcaster seulement si déjà registered (nick + user + pass déjà faits)
        if (client->isRegistered() && !oldNick.empty()) {
            std::string rep = ":" + oldNick + "!" + client->getUsername() 
                            + "@localhost NICK " + cmd.params[0] + "\r\n";
            send(client->getFd(), rep.c_str(), rep.size(), 0);
            // + broadcaster aux channels dont le client est membre
        }
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
	// 1. Verifier si le channel existe, si non il se cree automatiquement :
	bool isNewChannel = !server->doesChannelExist(channelName);
	Channel *channel = server->getOrCreateChannel(channelName);

	// 2. Ajouter le client au channel
	if (channel->isMember(client->getNickname()))
		return;
	//3. Si PAS nouveau channel → vérifier les modes +i, +k, +l
	if (!isNewChannel) {
		if (channel->isInviteOnly() && !channel->isInvited(client->getNickname())) {
			IRC::errInviteOnlyChan(client, channelName);
			return;
		}
		if (!channel->getKey().empty()) {
			if (cmd.params.size() < 2 || cmd.params[1] != channel->getKey()) {
				IRC::errBadChannelKey(client, channelName);
				return;
			}
		}
		if (channel->getUserLimit() > 0 && channel->getMembers().size() >= size_t(channel->getUserLimit())) {
			IRC::errChannelIsFull(client, channelName);
			return;
		}
	}
	//4. Si tout OK → addMember()
	channel->addMember(client);
	channel->removeInvited(client->getNickname());

	//5. Si nouveau channel → addOperator()
	if (isNewChannel)
		channel->addOperator(client);
	// 6. Notifier les autres membres du channel et lui-meme
	std::string JOINmsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN " + channelName + "\r\n";
	channel->broadcast(JOINmsg);

	// 7. Envoyer le topic s'il existe (code 332/331)
	if (channel->getTopic().empty())
		IRC::rplNoTopic(client, channelName);
	else
		IRC::rplTopic(client, channelName, channel->getTopic());

	// 8. Envoyer la liste des membres (le premier avec un '@') rpl 353,
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
	std::string ModesList = cmd.params[1];
	bool addModes = true;
	size_t paramsIdx = 2;

	for (size_t i = 0; i < ModesList.size(); i++) {
		char c = ModesList[i];
		if (c == '+') {
			addModes = true;
			continue; // Passe au caracteres suivant
		} else if (c == '-') {
			addModes = false;
			continue; // Passe au caracteres suivant
		}
		if (c == 'i') {
			channel->setInviteOnly(addModes);
		}
		else if (c == 't') {
			channel->setTopicRestricted(addModes);
		}
		else if (c == 'k') {
			if (addModes) {
				if (paramsIdx >= cmd.params.size())
					continue;
				channel->setKey(cmd.params[paramsIdx]);
				paramsIdx++;
			}
			else 
				channel->setKey("");
		}
		else if (c == 'o') {
			if (paramsIdx >= cmd.params.size()) // Verifie si index existant
					continue;
			std::string targetNick = cmd.params[paramsIdx]; // Puis on y accede
			paramsIdx++; // On incremente ici au cas ou s'il y a un probleme, le mode d'apres soit quand meme sur son bon parametres

			Client *target = server->getClientByNick(targetNick);
			if (!target || !channel->isMember(targetNick)) {
				std::cerr << "Error: " << targetNick << " is not on channel\r\n";
				continue;
			}
			if (addModes)
				channel->addOperator(target);
			else
				channel->removeOperator(target);
		}
		else if (c == 'l') {
			if (addModes) {
				if (paramsIdx >= cmd.params.size())
                	continue;

				int limit = atoi(cmd.params[paramsIdx].c_str());
				channel->setUserLimit(limit);
				paramsIdx++;
			}
		 	else
				channel->setUserLimit(0);
		}
	}
	std::string modeMsg = ":" + client->getNickname() + "!" + 
                      client->getUsername() + "@localhost MODE " + 
                      channelName + " " + ModesList + "\r\n";
	channel->broadcast(modeMsg);
}

void execNames(Client *client, const t_command &cmd, Server *server)
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
	
	std::string names = channel->getMembersList();
	IRC::rplNameReply(client, channelName, names);
	IRC::rplEndOfNames(client, channelName);
}


void	execKick(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;


	if (cmd.params.size() < 2) // verif le nb de param -> cf rfc -> Parameters: <channel> <user> [<comment>]
	{
		IRC::errMoreParams(client, cmd); // 461
		return;
	}

	std::string channelName = cmd.params[0];
	std::string targetNick  = cmd.params[1];


	Channel *channel = server->getChannel(channelName);
	if (!channel) // Verif que le channel existe bien
	{
		IRC::errNoSuchChannel(client, channelName); // 403
		return;
	}


    if (!channel->isMember(client->getNickname())) // verif que le client qui kick est ds le channel
	{
		IRC::errNotOnChannel(client, channelName); // 442
		return;
	}

	if (!channel->isOperator(client->getNickname()))
	{
		IRC::errOpNeededToChanges(client, channelName); // 482
		return;
	}


	Client *target = server->getClientByNick(targetNick); //verif que la cible  existe s/ le serveur
	if (!target)
	{
		IRC::errNoSuchNick(client, targetNick); // 401
		return;
	}

	if (!channel->isMember(targetNick))
	{
		IRC::errUserNotInChannel(client, targetNick, channelName); // 441
		return;
	}


	// Constru° du message KICK -> cf RFC 1459 -> Parameters: <channel> <user> [<comment>]

	std::string comment;
    
	if (cmd.params.size() > 2)
		comment = cmd.params[2];
	else
		comment = client->getNickname();

	std::string kickMsg = ":" + client->getNickname() + "!"
						+ client->getUsername() + "@localhost KICK "
						+ channelName + " " + targetNick
						+ " :" + comment + "\r\n";



	channel->broadcast(kickMsg); // diffuser le mess à ts les membres du channel
	server->handleClientLeavingChannel(target, channel, channelName);
}


void	execInvite(Client *client, const t_command &cmd, Server *server)
{
	if (!client -> isRegistered())
		return;

	if (cmd.params.size() < 2) // verif nb params -> cf rfc -> Parameters: <nickname> <channel>
	{
		IRC::errMoreParams(client, cmd); // 461
		return;
	}

	std::string targetNick = cmd.params[0];
	std::string channelName = cmd.params[1];


	Client *target = server->getClientByNick(targetNick); //verif que la cible existe bien
	if (!target)
	{
		IRC::errNoSuchNick(client, targetNick); // 401
		return;
	}


    Channel *channel = server->getChannel(channelName); // verif que le channel existe
	if (!channel)
	{
		IRC::errNoSuchChannel(client, channelName); // 403
		return;
	}


	if (!channel->isMember(client->getNickname())) //verif que l'orchestreur est déjà ds le channel
	{
		IRC::errNotOnChannel(client, channelName); // 442
		return;
	}


	if (channel->isMember(targetNick)) // au contR, verif que la cible n'est pas déjà ds le channel
    {
		IRC::errUserOnChannel(client, targetNick, channelName); // 443
		return;
	}


	if (channel->isInviteOnly() && !channel->isOperator(client->getNickname())) // si channel = +i (invite-only), vérif que l'orchestreur = opérateur
	{
		IRC::errOpNeededToChanges(client, channelName); // 482
		return;
	}

	channel->addInvited(targetNick); // ajout de la cible à la liste des invités


	IRC::rplInviting(client, targetNick, channelName); // confirma° à l'orchestreur


// notif
	std::string inviteMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost INVITE " + targetNick + " " + channelName + "\r\n";

	send(target->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);

}

void execQuit(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;

	std::string msg;
	if (cmd.params.empty())
		msg = "Client QUIT";
	else
		msg = cmd.params[0];

	std::string QuitMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost QUIT " + ":" + msg + "\r\n";

	std::map<std::string, Channel*> &channels = server->getChannels();
	std::map<std::string, Channel*>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->second->isMember(client->getNickname()))
			it->second->broadcast(QuitMsg);
	}
	server->removeClient(client->getFd());

}

void	execWho(Client *client, const t_command &cmd, Server *server)
{
	if (!client->isRegistered())
		return;
	if (cmd.params.empty())
		return;

	std::string channelName = cmd.params[0];
	Channel *channel = server->getChannel(channelName);

	if (!channel) {
		IRC::errNoSuchChannel(client, channelName);
		return;
	}

	const std::map<std::string, Client*> &members = channel->getMembers();
	for (std::map<std::string, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		Client *member = it->second;
		channel->isOperator(member->getNickname());
	    bool isOp = channel->isOperator(member->getNickname());
		IRC::rplWhoReply(client, channelName, member, isOp);
	}
	IRC::rplEndOfWho(client, channelName);
}