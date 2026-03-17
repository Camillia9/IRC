#ifndef EXECUTIONS_HPP
#define EXECUTIONS_HPP

#include "Client.hpp"
#include "Command.hpp"
#include "Replies.hpp"
#include "Channel.hpp"
#include <vector>
#include <string>

// Forward declaration de Server (pour éviter une dépendance circulaire)
class Server;

// Utils
bool isSpecialChar(char c);
bool isValidNick(const std::string &str);
bool isValidChannelName(const std::string &str);


// Authentification
void execPass(Client *client, const t_command &cmd, const std::string &password);
void execNick(Client *client, const t_command &cmd, const std::vector<Client*> &clients);
void execUser(Client *client, const t_command &cmd);

// Channels
void execJoin(Client *client, const t_command &cmd, Server *server);
void execPart(Client *client, const t_command &cmd, Server *server);

// Messages
void execPrvMsg(Client *client, const t_command &cmd, Server *server);

//Topic / Modes
void execTopic(Client *client, const t_command &cmd, Server *server);
void execModes(Client *client, const t_command &cmd, Server *server);

// KICK
void	execKick(Client *client, const t_command &cmd, Server *server);

// INVITE
void	execInvite(Client *client, const t_command &cmd, Server *server);

// QUIT
void	execQuit(Client *client, const t_command &cmd, Server *server);

// WHO
void	execWho(Client *client, const t_command &cmd, Server *server);
// Bonus
void execNames(Client *client, const t_command &cmd, Server *server);

#endif