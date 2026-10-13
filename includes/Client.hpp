#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class Client {
private :
	int			_fd; // fd de la socket client
	std::string	_recvBuffer; //Buffer pour accumuler les donnees recus
	std::string	_nickname; // Pseudo du client
	std::string	_username; // Username du client
	std::string	_realname; // Nom reel(optionnel)
	bool		_authenticated; // A fourni le bon mdp ?
	bool		_registered; // A complete NICK + USER ?

public:
	Client(int fd);
	~Client();

	// Getters
	int					getFd()const;
	const std::string&	getNickname()const;
	const std::string&	getUsername()const;
	const std::string&	getRealName()const;
	bool				isAuthenticated()const;
	bool				isRegistered()const;

	// Setters
	void	setNickname(const std::string& nickname);
	void	setUsername(const std::string& username);
	void	setRealname(const std::string& realname);
	void	setAuthenticated(bool auth);
	void	setRegistered(bool reg);
	
	// Gestion du buffer
	void						appendToBuffer(const std::string& data);
	std::vector<std::string>	extractMessages();
	void						clearBuffer();
};

#endif