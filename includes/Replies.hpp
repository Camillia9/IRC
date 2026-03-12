#ifndef REPLIES_HPP
#define REPLIES_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Command.hpp"
#include "Server.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

namespace IRC {
	// Utils
	std::string getDisplayNick(Client *client);
	
	// Enregistrement (NICK/USER/PASS)
	void sendWelcome(Client *client);
	void errMoreParams(Client *client, const t_command command);
	void errAlreadyReg(Client *client);
	void errPassMismatch(Client *client);
	void errNoNickGiven(Client *client);
	void errNickChar(Client *client);
	void errNickInuse(Client *client, const std::string &nick);

	// TOPIC / PART
	void rplNoTopic(Client *client, const std::string &chanel);
	void rplTopic(Client *client, const std::string &chanel, const std::string &topic);
	void rplNameReply(Client *client, const std::string &chanel, const std::string &name);
	void rplEndOfNames(Client *client, const std::string &chanel);
	void errNotOnChannel(Client *client, const std::string &chanel);
	void errNoSuchChannel(Client *client, const std::string &chanel);

	// PRIVMSG
	void errNoRecipient(Client *client, const t_command &command);
	void errNoTextSend(Client *client);
	void errNoSuchNick(Client *client, const std::string &target);
	void errCannotSendToChan(Client *client, const std::string &channel);

	// MODES
	void errOpNeededToChanges(Client *client, const std::string &channel);
	void rplChannelMode(Client *client, const std::string &channel, const std::string &modes);
	void errChannelIsFull(Client *client, const std::string &channel);
	void errInviteOnlyChan(Client *client, const std::string &channel);
	void errBadChannelKey(Client *client, const std::string &channel);

	//KICK
	void errUserNotInChannel(Client *client, const std::string &nick, const std::string &channel);

	// INVITE
	void rplInviting(Client *client, const std::string &nick, const std::string &channel);
	void errUserOnChannel(Client *client, const std::string &nick, const std::string &channel);

	// WHO
	void rplWhoReply(Client *client, const std::string &channelName, Client *member, bool isOp);
	void rplEndOfWho(Client *client, const std::string &channel);


};	

#endif