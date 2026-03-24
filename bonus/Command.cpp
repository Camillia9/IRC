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

//void Bot::cmdInfo(const std::string &target)
//{
//	std::vector<std::string>info;

//	info.push_back("\n=== IRC Commands References ===\n ~~JOIN~~\n JOIN #channel ou /join channel\n ~~PART~~\nPart #channel [:reason] ou /part [:reason]");


//	sendMultilineMessage(target, info);

//}

void Bot::cmdInfo(const std::string &target)
{
    std::string guide;
    
    guide += "\n=================================\n";
    guide += "    IRC Commands Reference\n";
    guide += "=================================\n\n";
    
    guide += "   > Join a channel:\n";
    guide += "   nc: JOIN #channel\n";
    guide += "   irssi: /join #channel\n\n";
    
    guide += "   > Leave a channel:\n";
    guide += "   nc: PART #channel [:reason]\n";
    guide += "   irssi: /part #channel [:reason]\n\n";
    
    guide += "   > Send a message:\n";
    guide += "   nc: PRIVMSG user :msg\n";
    guide += "   irssi: /msg user msg\n\n";

	guide += "   > Topic of the channel\n";
	guide += "   nc: Topic #channel [:new Topic]\n";
	guide += "   irssi: /Topic [new Topic]\n";

	guide += "   > Kick a user from channel:\n";
    guide += "   nc: KICK #channel user [:reason]\n";
    guide += "   irssi: /kick user [:reason]\n\n";

    guide += "   > Invite a user to channel:\n";
    guide += "   nc: INVITE user #channel\n";
    guide += "   irssi: /invite user\n\n";

    guide += "   > Disconnect from server:\n";
    guide += "   nc: QUIT [:message]\n";
    guide += "   irssi: /quit [:message]\n\n";

    guide += "   > Display channel members:\n";
    guide += "   nc: NAMES #channel\n";
    guide += "   irssi: /names\n\n";
    
	guide += "   > Channel modes:\n";
    guide += "   nc: MODE #channel [+/-mode] [params]\n";
    guide += "   irssi: /mode #channel [+/-mode] [params]\n\n";
    guide += "     Display modes: MODE #channel\n";
    guide += "     +t : Only ops can change topic\n";
    guide += "     +i : Invite-only (only invited users can join)\n";
    guide += "     +k <key> : Channel password required\n";
    guide += "     +o <nick> : Give operator status\n";
    guide += "     +l <limit> : Set user limit\n\n";
    
    guide += "=================================";
    
    sendMessage("PRIVMSG " + target + " :" + guide + "\r\n");

    
}

