#include "Server.hpp"

bool g_running = true;

void signalHandler(int signal)
{
    (void)signal;
    g_running = false;
}

int main(int ac, char **av)
{
	signal(SIGINT, signalHandler);
	if (ac != 3) {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port = std::atoi(av[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: Invalid port number" << std::endl;
		return 1;
	}

	std::string password = av[2];

	try
	{
		Server server(port, password);
		server.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;

}