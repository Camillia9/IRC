#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/socket.h>  // Pour socket(), bind(), listen(), accept()
#include <netinet/in.h>  // Pour struct sockaddr_in
#include <unistd.h>      // Pour close()
#include <cstring>		// Pour memset()

class Server {
private:
	int					_port;			// Le port sur lequel écouter (ex: 6667)
	std::string			_password;		// Le mot de passe de connexion que les clienst devront fournir
	int					_serverSocket;	// Le "numéro" de la socket serveur (c'est un fd)
	struct sockaddr_in	_serverAddr;	// L'adresse réseau du serveur

public:
	Server(int port, const std::string &password);	// Constructeur
	~Server();										// Destructeur

	void start();					// Lancement complet du processus
	void createSocket();			// 1: Creation socket
	void bindSocket();				// 2: lier au port
	void listenForConnections();	// 3: mettre en ecoute
	void acceptConnections();		// 4: accepter UN client
};

#endif