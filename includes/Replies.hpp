#ifndef REPLIES_HPP
#define REPLIES_HPP

#include "Client.hpp"
#include "Command.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

namespace IRC {
	std::string getDisplayNick(Client *client);
	
	void sendWelcome(Client *client);
	void errMoreParams(Client *client, const t_command command);
	void errAlreadyReg(Client *client);
	void errPassMismatch(Client *client);
	void errNoNickGiven(Client *client);
	void errNickChar(Client *client);
	void errNickInuse(Client *client, const std::string &nick);
};

#endif