#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Replies.hpp"
#include "parser.hpp"
#include "Execution.hpp"
#include "Channel.hpp"


#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/socket.h>  // Pour socket(), bind(), listen(), accept()
#include <netinet/in.h>  // Pour struct sockaddr_in
#include <unistd.h>      // Pour close()
#include <cstring>		// Pour memset()
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <map>



class Server {
private:
	int						_port;			// Le port sur lequel écouter (ex: 6667)
	std::string				_password;		// Le mot de passe de connexion que les clienst devront fournir
	int						_serverSocket;	// Le "numéro" de la socket serveur (c'est un fd)
	struct sockaddr_in		_serverAddr;	// L'adresse réseau du serveur
	std::vector<Client*>    _clients;      // Liste de tous les clients
    std::vector<pollfd>     _pollFds;      // Liste des file descriptors à surveiller

	void createSocket();			// 1: Creation socket
	void bindSocket();				// 2: lier au port
	void listenForConnections();	// 3: mettre en ecoute
	void initPoll();
	void acceptNewClient();   		// Nouvelle connexion détectée
	void handleClientData(int fd);  // Client a envoyé des données
	void removeClient(int fd);      // Client déconnecté
	void addClient(int fd);			// Ajouter un client à la liste
	Client*  getClientByFd(int fd); // Retrouver un client par son FD
	//Client*	getClientByNick(const std::string &nick); // Retrouver un client par son nick

	std::map<std::string, Channel*> _channels; // ts les channels



	//gest° channel
	bool isValidChannelName(const std::string& channelName) const;
	
	
	
	
	
	
	
	public:
	Server(int port, const std::string &password);	// Constructeur
	~Server();										// Destructeur
	
	void run();               		// Boucle principale
	
	const std::string&	getPassword()const;
	const std::vector<Client*>& getClients()const;
	
	Client*	getClientByNick(const std::string &nick); // Retrouver un client par son nick
	void executeCommand(Client &client, const t_command &cmd, Server *server);
	
	Channel*	findChannel(const std::string& channelName);
    Channel*	createChannel(const std::string& channelName);
    Channel*	getOrCreateChannel(const std::string& channelName);
	Channel* 	getChannel(const std::string &name);
    void	deleteChannelIfEmpty(const std::string& channelName);
	bool doesChannelExist(const std::string& channelName);
};



#endif