#include "Execution.hpp"

bool isSpecialChar(char c) {
    return std::string("[]{}|\\").find(c) != std::string::npos;
}

bool isValidNick(const std::string &str) {
    if (!isalpha(str[0]) && !isSpecialChar(str[0]))
        return false;
    for (size_t i = 1; i < str.size(); i++)
        if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-')
            return false;
    return true;
}

bool isValidChannelName(const std::string &str) {
	if (str.empty() || str[0] != '#')
        return false;
	if (str.size() < 2)
		return false;
	for (size_t i = 1; i < str.size(); i++) {
        if (!isalnum(str[i]) && str[i] != '_' && str[i] != '-')
            return false;
    }
	return true;
}

std::string Channel::getModesString()const 
{
	std::string modes = "+";
	std::string params = "";

	if (_inviteOnly)
		modes += "i";
	if (_topicRestricted)
		modes += "t";
	if (!_key.empty()) {
		modes += "k";
		params += " " + _key;
	}
	if (_userLimit > 0) {
		modes += "l";
		std::ostringstream oss;
		oss << _userLimit;
		params += " " + oss.str();
	}

	return modes + params;
}