#include "Replies.hpp"

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