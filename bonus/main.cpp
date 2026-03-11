#include "bot.hpp"

int main(int ac, char **av)
{
	if (ac != 5) {
		std::cerr << "Usage: ./ircbot <host> <port> <password> <nickname>" << std::endl;
		return 1;
	}
	Bot bot(av[4]);
	bot.connect(av[1], atoi(av[2]), av[3]);
	bot.joinChannel("#bot");
	bot.run();
	
	return 0;
}