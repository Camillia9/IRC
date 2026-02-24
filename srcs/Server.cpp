#include "Server.hpp"

extern bool g_running;

Server::Server(int port, const std::string &password) : _port(port), _password(password), _serverSocket(-1)
{
	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
}

Server::~Server()
{
	for (size_t i = 0; i < _clients.size(); i++) {
		close(_clients[i]->getFd());
		delete _clients[i];
	}
	if (_serverSocket != -1)
		close(_serverSocket);
}

void Server::createSocket()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
		throw std::runtime_error("Failed to create socket");
	std::cout << "OK, Socket created" << std::endl;
	fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
}

void Server::bindSocket()
{
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(_port);

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockpot failed");
	
	if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throw std::runtime_error("Failed to bind socket");

	std::cout << "OK, Socket bound to port " << _port << std::endl;
}

void Server::listenForConnections()
{
	if (listen(_serverSocket, 10) < 0)
		throw std::runtime_error("Failed to listen on socket");
	
	std::cout << "OK, Server listening on port " << _port << std::endl;
}

void Server::initPoll()
{
	pollfd PollFds;
	PollFds.fd = _serverSocket;
	PollFds.events = POLLIN;
	PollFds.revents = 0;

	_pollFds.push_back(PollFds);
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	
	int clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientSocket < 0)
	throw std::runtime_error("Failed to accept connection");
	
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	std::cout << "OK, New connection accepted (FD: " << clientSocket << ")" << std::endl;
	
	addClient(clientSocket);
}

void Server::addClient(int fd)
{
	Client *newClient = new Client(fd);
	_clients.push_back(newClient); // Ajoute le nouveau client dans le vector contenant la liste de tout les clients

	pollfd pollFds;
	pollFds.fd = fd;
	pollFds.events = POLLIN;
	pollFds.revents = 0;

	_pollFds.push_back(pollFds);

	std::cout << "New clients fd " << fd << " added" << std::endl;
}

void Server::removeClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]->getFd() == fd) {
			delete _clients[i]; // delete car allocation de memoire avec new, du coup pointeur = NULL
			_clients.erase(_clients.begin() + i); // Erase Supprime l'element i (le pointeur a NUL)
			break;
		}
	}
	for (size_t i = 0; i < _pollFds.size(); i++) {
		if (_pollFds[i].fd == fd) {
			_pollFds.erase(_pollFds.begin() + i); //PollFds est une structure simple, pas d'allocation de memoire
			break;
		}
	}
	std::cout << "Clients fd " << fd << " removed" << std::endl;
	close(fd);
}

const std::string& Server::getPassword()const 
{
	return _password;
}

const std::vector<Client*>& Server::getClients()const
{
	return _clients;
}

Client *Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]->getFd() == fd)
			return _clients[i];
		}
		return NULL;
}

// Client *Server::getClientByNick(const std::string &nick)
// {

// }

void Server::handleClientData(int fd)
{
	Client *client = getClientByFd(fd);
	if (!client) 
	{
		std::cerr << "Client not found" << std::endl;
		return;
	}
	char buffer[1024]; //
	std::memset(buffer, 0, sizeof(buffer));

	int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
	
	if (bytesRead > 0) 
	{
		client->appendToBuffer(buffer);
		
		std::vector<std::string> msg = client->extractMessages();
		
		for (size_t i = 0; i < msg.size(); i++)
			std::cout << msg[i] << std::endl;
	

	//	for (size_t j = 0; j < msg.size(); ++j)
	//	{
	//	    std::cout << "💬 Client FD " << fd << " sent: " << msg[j] << std::endl;
		
	//	    t_command cmd = parseMessage(msg[j]);
	//	    executeCommand(client, cmd);
	//	}

	}

	else 
	{
		removeClient(fd);
	}

}

void Server::run()
{
	createSocket();
	bindSocket();
	listenForConnections();
	initPoll();

	std::cout << "Server running... Press Ctrl+C to stop" << std::endl;
	while (g_running) {
		int pollEvents = poll(_pollFds.data(), _pollFds.size(), -1); // Attendre un event sur nimporte quel socket

		if (pollEvents < 0) {
			std::cerr << "poll() error" << std::endl;
			break;
		}
		for (size_t i = 0; i < _pollFds.size(); i++) { // Parcourir tout les fd
			if (_pollFds[i].revents == 0) // Rien a faire
				continue;

			if (_pollFds[i].revents & POLLIN) { // Donnes a lire ?
				if (_pollFds[i].fd == _serverSocket) // C'est le serveur donc nouvelle connexion
					acceptNewClient();
				else
					handleClientData(_pollFds[i].fd); // c'est un client donc donnee recu
			}
			if (_pollFds[i].revents & (POLLERR | POLLHUP)) { // Client deconnecte ?
				std::cout << "Client Disconnected" << std::endl;
				removeClient(_pollFds[i].fd);
				i--;
			}
		}
	}
	
}