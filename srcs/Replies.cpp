#include "Replies.hpp"

std::string IRC::getDisplayNick(Client *client)
{
	if (client->getNickname().empty())
		return "*";
	return client->getNickname();
}

void IRC::sendWelcome(Client *client)
{
	std::ostringstream oss;
	oss << ":ircserv 001 " << client->getNickname() << " :Welcome to the Internet Relay Network "
	<< client->getNickname() << "!" << client->getUsername() << "@localhost\r\n";
	
	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errMoreParams(Client *client, const 	t_command command)
{
	std::string nick = getDisplayNick(client);

	std::ostringstream oss;
	oss << ":ircserv 461 " << nick << " " << command.command << " :Not enough parameters\r\n";
	
	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errAlreadyReg(Client *client)
{
	std::ostringstream oss;
	oss << ":ircserv 462 " << client->getNickname() << " :Unauthorized command (already registered)\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errPassMismatch(Client *client)
{
	std::string nick = getDisplayNick(client);

	std::ostringstream oss;
	oss << ":ircserv 464 " << nick << " :Password incorrect\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNoNickGiven(Client *client)
{
	std::string nick = getDisplayNick(client);

	std::ostringstream oss;
	oss << ":ircserv 431 " << nick << " :No nickname given\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNickChar(Client *client)
{
	std::string nick = getDisplayNick(client);

	std::ostringstream oss;
	oss << ":ircserv 432 " << nick << " :Erroneous nickname\r\n";
	
	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNickInuse(Client *client, const std::string &nick)
{
	std::ostringstream oss;
	oss << ":ircserv 433 " << client->getNickname() << " " << nick << " :Nickname is already in use\r\n";
	
	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}