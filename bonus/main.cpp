#include "bot.hpp"

bool g_running = true;

void signalHandler(int signal)
{
    (void)signal;
    g_running = false;
}

int main(int ac, char **av)
{
	signal(SIGINT, signalHandler);
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