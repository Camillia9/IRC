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