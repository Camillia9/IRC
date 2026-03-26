#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <vector>
#include <errno.h>

class Bot {
private:
	int			_socket;
	std::string	_nick;
	std::string _realName;
	std::string _recvBuffer;

	void sendMessage(const std::string &msg);
	void handleMessage(const std::string &line);
	void sendMultilineMessage(const std::string &target, const std::vector<std::string> &lines);
	void handleCommand(const std::string &from, const std::string &target, const std::string &message);
	void cmdHelp(const std::string &target);
	void cmdTime(const std::string &target);
	void cmdPing(const std::string &target, const std::string &from);
	void cmdInfo(const std::string &target);
	
	public:
	Bot(const std::string &nickname);
	~Bot();
	
	void connect(const std::string &host, int port, const std::string &pass);
	void authenticated(const std::string &pass);
	std::string receiveMessage();
	void joinChannel(const std::string &channel);
	void run();
};

#endif