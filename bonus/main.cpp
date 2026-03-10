#include "bot.hpp"

int main()
{
	Bot bot("BOTtest");
	bot.connect("127.0.0.1", 6667, "mdp");

	std::cout << "Ok: Bot connecetd success" << std::endl;
	
	return 0;
}