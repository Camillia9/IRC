#include "bot.hpp"

Bot::Bot(const std::string &nickname) : _nick(nickname), _realName("BOT Serv") {}

Bot::~Bot() {}

void Bot::connect(const std::string &host, int port, const std::string &pass)
{
	_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (_socket == -1) {
		std::cerr << "Error: socket() failed" << std::endl;
		exit(1);
	}
	std::cout << "OK: Socket created: " << _socket << std::endl;

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(host.c_str());

	std::cout << "Server adress configured: " << host << ":" << port << std::endl;

	if (::connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "Error: connect() failed" << std::endl;
		close(_socket);
		exit(1);
	}
	std::cout << "OK: Connected to server!" << std::endl;

	authenticated(pass);

}

void Bot::sendMessage(const std::string &msg)
{
    send(_socket, msg.c_str(), msg.size(), 0);
    std::cout << ">> " << msg;  // Debug
}

void Bot::authenticated(const std::string &pass)
{
	sendMessage("PASS " + pass + "\r\n");
	sendMessage("NICK " + _nick + "\r\n");
	sendMessage("USER " + _nick + " 0 * :" + _realName + "\r\n");

	std::cout << "OK: Authentication sent!" << std::endl;
}