#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd),
	_recvBuffer(""),
	_nickname(""),
	_username(""),
	_realname(""),
	_authenticated(false),
	_registered(false)
{
	std::cout << "OK, Client created (FD: " << _fd << ")" << std::endl;
}

Client::~Client()
{
	std::cout << "Client destroyed (FD: " << _fd << ")" << std::endl;
}

// Guetters
int Client::getFd() const
{
    return _fd;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

const std::string& Client::getRealName()const
{
	return _realname;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

bool Client::isRegistered() const
{
    return _registered;
}

// Setters
void Client::setNickname(const std::string& nickname)
{
	_nickname = nickname;
	std::cout << "Client FD " << _fd << " nickname set to: " << nickname << std::endl;
}

void Client::setUsername(const std::string& username)
{
	_username = username;
	std::cout << "Client FD " << _fd << " username set to: " << username << std::endl;
}

void Client::setRealname(const std::string& realname)
{
	_realname = realname;
	std::cout << "Client FD " << _fd << " real name set to: " << realname << std::endl;
}

void Client::setAuthenticated(bool auth)
{
	_authenticated = auth;
	if (auth)
		std::cout << "Client FD " << _fd << " authenticated" << std::endl;
}

void Client::setRegistered(bool reg)
{
	_registered = reg;
	if (reg)
		std::cout << "client FD " << _fd << " fully registered" << std::endl;
}

void Client::appendToBuffer(const std::string& data)
{
	_recvBuffer += data;
	//std::cout << "Buffer FD " << _fd << " (" << _recvBuffer.size() << " bytes: [" << _recvBuffer << "]" << std::endl;
}

std::vector<std::string> Client::extractMessages()
{
	std::vector<std::string> messages;
	size_t pos;

	while((pos = _recvBuffer.find("\r\n")) != std::string::npos)
	{
		std::string msg = _recvBuffer.substr(0, pos);
		if (!msg.empty())
		{
			messages.push_back(msg);
			std::cout << "Message extracted: [" << msg << "]" << std::endl;
		}
		_recvBuffer.erase(0, pos + 2);
	}
	return messages;
}

void Client::clearBuffer()
{
    _recvBuffer.clear();
}