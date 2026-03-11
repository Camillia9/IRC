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

void IRC::rplNoTopic(Client *client, const std::string &chanel)
{
	std::ostringstream oss;
	oss << ":ircserv 331 " << client->getNickname() << " " << chanel << " :No topic is set\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::rplTopic(Client *client, const std::string &chanel, const std::string &topic)
{
	std::ostringstream oss;
	oss << ":ircserv 332 " << client->getNickname() << " " << chanel << " :" << topic << "\r\n";
	
	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::rplNameReply(Client *client, const std::string &chanel, const std::string &names)
{
	std::ostringstream oss;
	oss << ":ircserv 353 " << client->getNickname() << " = " << chanel << " :" << names << "\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::rplEndOfNames(Client *client, const std::string &chanel)
{
	std::ostringstream oss;
	oss << ":ircserv 366 " << client->getNickname() << " " << chanel << " :End of /NAMES list\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNotOnChannel(Client *client, const std::string &chanel)
{
	std::ostringstream oss;
	oss << ":ircserv 442 " << client->getNickname() << " " << chanel << " :You're not on that chanel\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNoSuchChannel(Client *client, const std::string &chanel)
{
	std::ostringstream oss;
	oss << ":ircserv 403 " << client->getNickname() << " " << chanel << " :No such chanel\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errNoRecipient(Client *client, const t_command &command)
{
	std::ostringstream oss;
	oss << ":ircserv 411 " << client->getNickname() << " :No recipient given (" << command.command << ")\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errNoTextSend(Client *client)
 {
	std::ostringstream oss;
	oss << ":ircserv 412 " << client->getNickname() << " :No text to send\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errNoSuchNick(Client *client, const std::string &target)
 {
	std::ostringstream oss;
	oss << ":ircserv 401 " << client->getNickname() << " " << target << " :No such nick/channel\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errCannotSendToChan(Client *client, const std::string &channel)
 {
	std::ostringstream oss;
	oss << ":ircserv 404 " << client->getNickname() << " " << channel << " :Cannot send to channel\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errOpNeededToChanges(Client *client, const std::string &channel)
 {
	std::ostringstream oss;
	oss << ":ircserv 482 " << client->getNickname() << " " << channel << " :You're not channel operator\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::rplChannelMode(Client *client, const std::string &channel, const std::string &modes)
 {
	std::ostringstream oss;
	oss << ":ircserv 324 " << client->getNickname() << " " << channel << " " << modes << "\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errChannelIsFull(Client *client, const std::string &channel)
 {
	std::ostringstream oss;
	oss << ":ircserv 471 " << client->getNickname() << " " << channel << " :Cannot join channel (+l)\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);	
 }

 void IRC::errInviteOnlyChan(Client *client, const std::string &channel)
 {
	std::ostringstream oss;
	oss << ":ircserv 473 " << client->getNickname() << " " << channel << " :Cannot join channel (+i)\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

 void IRC::errBadChannelKey(Client *client, const std::string &channel)
 {
	std::ostringstream oss;
	oss << ":ircserv 475 " << client->getNickname() << " " << channel << " Cannot join channel (+k)\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
 }

void IRC::errUserNotInChannel(Client *client, const std::string &nick, const std::string &channel) // utilise ds kick
{
	std::ostringstream oss;
	oss << ":ircserv 441 " << client->getNickname() << " " << nick << " " << channel << " :They aren't on that channel\r\n";

	std::string rep = oss.str();
	send(client->getFd(), rep.c_str(), rep.size(), 0);
}

// USE ds INVITE

void IRC::rplInviting(Client *client, const std::string &nick, const std::string &channel)
{
    std::ostringstream oss;
    oss << ":ircserv 341 " << client->getNickname() << " " << nick << " " << channel << "\r\n";
    
    std::string rep = oss.str();
    send(client->getFd(), rep.c_str(), rep.size(), 0);
}

void IRC::errUserOnChannel(Client *client, const std::string &nick, const std::string &channel)
{
    std::ostringstream oss;
    oss << ":ircserv 443 " << client->getNickname() << " " << nick << " " << channel 
        << " :is already on channel\r\n";
    
    std::string rep = oss.str();
    send(client->getFd(), rep.c_str(), rep.size(), 0);
}