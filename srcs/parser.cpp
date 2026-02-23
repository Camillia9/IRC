#include "parser.hpp"
// #include "Command.hpp"

std::string	read_word(const std::string &line, size_t &i)
{
	std::string word;

	skip_spaces(line, i);

	if (i >= line.size())
		return word;

	if (line[i] == ':') //si direct commence par ":"
	{
		i++; // skip ':'
		while (i < line.size())
		{
			word += line[i];
			i++;
		}
		
		return word;
    }

	while (i < line.size() && line[i] != ' ') //cas basique 
	{
		word += line[i];
		i++;
	}
	
	return word;
}


void	skip_spaces(const std::string &line, size_t &i)
{
	while (i < line.size() && line[i] == ' ')
		i++;
}


std::string	read_id(const std::string &line, size_t &i)
{
	std::string	id;

	if (i >= line.size())
		return id;
	
	if (line[i] != ':')
		return id;
	i++; //pr skip le ":"

	//while (i < line.size() && line[i] != ' ')
	//{
		//id += line[i];
		//i++;


	//}

	//return id;
	return read_word(line, i);
}

std::vector<std::string>	read_param(const std::string &line, size_t &i)
{
	std::vector<std::string> params;

	while (i < line.size())
	{
		skip_spaces(line, i);

		if (i >= line.size())
			break;

		// cas où last param avec ':'
		if (line[i] == ':')
		{
			i++; // skip ':'
			//params.push_back(line.substr(i));
			//break;
			std::string last = line.substr(i);

			while (!last.empty() && (last.back() == '\r' || last.back() == '\n'))
				last.pop_back();

			params.push_back(last);
			break;
		}

		// param normal
		params.push_back(read_word(line, i));

		
		if (params.size() >= 15)//limite RFC : 15 params askip
			break;
	}

	return params;
}


std::string	read_command(const std::string &line, size_t &i)
{
	return read_word(line, i);
}


MessClient parse_line(const std::string &line)
{
	MessClient	msg;
	size_t	i = 0;

	msg.brut = line; msg.id = read_id(line, i);
	msg.command = read_command(line, i);
	std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
	msg.params = read_param(line, i);

	return msg;
}
