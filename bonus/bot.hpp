#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

class Bot {
private:
	int			_socket;
	std::string	_nick;
	std::string _realName;

	void sendMessage(const std::string &msg);

public:
	Bot(const std::string &nickname);
	~Bot();

	void connect(const std::string &host, int port, const std::string &pass);
	//void sendMessage(const std::string &msg);
	void authenticated(const std::string &pass);
};

#endif