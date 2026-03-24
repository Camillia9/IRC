#include "bot.hpp"

void Bot::cmdHelp(const std::string &target)
{
    sendMessage("PRIVMSG " + target + " :Available commands: !help, !time, !ping, !info\r\n");
}

void Bot::cmdTime(const std::string &target)
{
    time_t now = time(0); // recup le temps en seconde.
    std::string timeStr = ctime(&now); // On le convertit en heure lisible
    
    // ctime() renvoie une chaine avec un '\n' à la fin, donc on l'enleve 
    if (!timeStr.empty() && timeStr[timeStr.size() - 1] == '\n')
        timeStr.erase(timeStr.size() - 1);
    
    sendMessage("PRIVMSG " + target + " :Current time: " + timeStr + "\r\n");
}

void Bot::cmdPing(const std::string &target, const std::string &from)
{
    sendMessage("PRIVMSG " + target + " :" + from + ": pong!\r\n");
}

void Bot::cmdInfo(const std::string &target)
{
	std::vector<std::string>info;

	info.push_back("=== IRC Commands References ===");

	info.push_back("JOIN (Join a channel)");
	info.push_back("Netcat: JOIN #channel");
	info.push_back("IRSSI: /JOIN channel");

	info.push_back("PART (Leave a channel)");
	info.push_back("Netcat: PART #channel [: reason]");
	info.push_back("IRSSI: /PART [reason]");

	sendMultilineMessage(target, info);

}
