#ifndef EXECUTIONS_HPP
#define EXECUTIONS_HPP

#include "Client.hpp"
#include "Command.hpp"
#include <vector>
#include <string>

// Forward declaration de Server (pour éviter une dépendance circulaire)
class Server;

// Authentification
void execPass(Client *client, const t_command &cmd, const std::string &password);
void execNick(Client *client, const t_command &cmd, const std::vector<Client*> &clients);
void execUser(Client *client, const t_command &cmd);

// Channels
void execJoin(Client *client, const t_command &cmd, Server *server);
void execPart(Client *client, const t_command &cmd, Server *server);

// Messages
void execPrivmsg(Client *client, const t_command &cmd, Server *server);

#endif