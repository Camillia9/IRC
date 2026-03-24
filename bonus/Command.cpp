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
	std::string info[] = {
		"Si tu vois ce message, c'est que le code marche. Incroyable.",
		"Tu avais une vie avant de lancer ce programme.",
		"404: Motivation not found.",
		"Ton futur toi te regarde et dit: 'frere... pourquoi ?'",
		"Ce programme est plus stable que toi.",
		"Tu pensais faire quelque chose d'utile aujourd'hui ?",
		"Ton PC chauffe plus que tes relations.",
		"Le bug est une feature. Toujours.",
		"Bravo. Tu viens de perdre 3 secondes de ta vie.",
		"Le problème, c'est pas le code. C'est toi.",
		"Compilation réussie. Ta vie, moins.",
		"Ce message n'a aucune utilité. Comme toi.",
		"Tu vas relancer le programme. On le sait tous.",
		"Mon code fonctionne. Tu peux maintenant tout casser.",
	};

	int random = rand() % 14;
    sendMessage("PRIVMSG " + target + " :🎱 " + info[random] + "\r\n");
}
