#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

typedef struct s_command
{
	std::string					id;
	std::string 				command; // PASS, NICK ...
	std::vector<std::string>	params; // ["Alice", "0", "*"], ["secret"] ...
	std::string					brut; //Debug
}	t_command;

#endif