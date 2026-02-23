#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

struct MessClient
{
	std::string id;
	std::string command;
	std::vector<std::string> params;
	std::string brut; //pr debugg pcq I'm just a girl

};


MessClient parse_line(const std::string &line);
std::string	read_word(const std::string &line, size_t &i);
void	skip_spaces(const std::string &line, size_t &i);
std::string	read_id(const std::string &line, size_t &i);
std::vector<std::string>	read_param(const std::string &line, size_t &i);
std::string	read_command(const std::string &line, size_t &i);

















#endif