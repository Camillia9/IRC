#include <iostream>

#include "parser.hpp"

int main()
{
	std::string line;

	std::cout << "TESTONS LE PARSER MA GO" << std::endl;

	while (true)
	{
		std::cout << "\nTape une ligne IRC :" << std::endl;

		if (!std::getline(std::cin, line))
			break;

		MessClient msg = parse_line(line);

		std::cout << "\n--- RESULTAT ---" << std::endl;

		std::cout << "brut    : [" << msg.brut << "]" << std::endl;
		std::cout << "id      : [" << msg.id << "]" << std::endl;
		std::cout << "command : [" << msg.command << "]" << std::endl;

		std::cout << "params  : ";
		if (msg.params.empty())
    		std::cout << "(none)";
		else
			for (size_t i = 0; i < msg.params.size(); i++)
				std::cout << "[" << msg.params[i] << "] ";

		std::cout << std::endl;
	}
}

