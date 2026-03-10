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

	std::map<std::string, Channel*>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;

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
	// 1. Trouver le client
	Client *client = getClientByFd(fd);
	if (!client)
		return; // Client deja supp ou introuvables

	std::cout << "REMOVING client: " << client->getNickname() 
       << " (FD " << fd << ")" << std::endl;
	
	// 2. Le retirer de tous ses channels
	std::vector<std::string> channelsToDelete; // vector pour push tout les channels a supprimer (pour eviter de supprimer directment en parcourant les channels et que l'it devient invalide)
	std::vector<std::string>channelsToHandle; // Pareil pour la gestion des operators
	std::map<std::string, Channel*>::iterator it;

	for (it = _channels.begin(); it != _channels.end(); ++it) {
		std::string channelName = it->first;
		Channel *channel = it->second;
		std::cout << "   Checking channel: " << it->first << std::endl;

		if (it->second->isMember(client->getNickname())) {
			channelsToHandle.push_back(it->first);
		}
		
		if (channel->isEmpty()) {
			std::cout << "   Client is member, removing..." << std::endl;
			channelsToDelete.push_back(channelName);
		}
	}
	std::cout << " Client deleted" << std::endl;
	for (size_t i = 0; i < channelsToDelete.size(); i++)
		this->deleteChannelIfEmpty(channelsToDelete[i]);
	
	for (size_t i = 0; i < channelsToHandle.size(); i++) {
		Channel *channel = getChannel(channelsToHandle[i]);
		if (channel)
			handleClientLeavingChannel(client, channel, channelsToHandle[i]);
	}

	// 3. Le retirer de _clients
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] == client) {
			_clients.erase(_clients.begin() + i); // Erase Supprime l'element i (le pointeur a NUL)
			delete client; // delete car allocation de memoire avec new, du coup pointeur = NULL
			break;
		}
	}

	// 4. Le retirer de _pollFds
	for (size_t i = 0; i < _pollFds.size(); i++) {
		if (_pollFds[i].fd == fd) {
			_pollFds.erase(_pollFds.begin() + i); //PollFds est une structure simple, pas d'allocation de memoire
			break;
		}
	}

	// 5. Fermer la socket
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

Client *Server::getClientByNick(const std::string &nick)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getNickname() == nick)
			return _clients[i];
	}
	return NULL;
}

void	Server::executeCommand(Client &client, const t_command &cmd, Server *server)
{

	const std::string &name = cmd.command;

	if (name.empty())
		return;

	if (name == "PASS")
		execPass(&client, cmd, _password);

	else if (name == "NICK")
		execNick(&client, cmd, _clients);

	else if (name == "USER")
		execUser(&client, cmd);

	else if (name == "JOIN")
	 	execJoin(&client, cmd, server);

	else if (name == "PART")
	 	execPart(&client, cmd, server);
	
	else if (name == "PRIVMSG")
		execPrvMsg(&client, cmd, server);

	else if (name == "TOPIC")
		execTopic(&client, cmd, server);

	else if (name == "MODE")
		execModes(&client, cmd, server);

	else if (name == "NAMES")
		execNames(&client, cmd, server);

	else
		std::cout << "Unknown command : " << name << std::endl;;
}

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
			// std::cout << msg[i] << std::endl;
		{

			t_command cmd = parse_line(msg[i]);

			// cmd.brut = line;

			executeCommand(*client, cmd, this);
        }
	}
	else
	    removeClient(fd);
}

void Server::handleClientLeavingChannel(Client *client, Channel *channel, const std::string &channelName)
{
	bool wasOperator = channel->isOperator(client->getNickname());

	channel->removeMember(client);
	
	if (wasOperator && !channel->isEmpty()) {
		const std::map<std::string, Client*> &Member = channel->getMembers();
		std::map<std::string, Client*>::const_iterator it = Member.begin();
		if (it != Member.end()) {
			Client *newOp = it->second;
			channel->addOperator(newOp);
		}
	}
	if (channel->isEmpty())
		deleteChannelIfEmpty(channelName);
}


void Server::run()
{
	createSocket();
	bindSocket();
	listenForConnections();
	initPoll();

	std::cout << "Server running... Press Ctrl+C to stop" << std::endl;
	while (g_running)
	{
		int pollEvents = poll(_pollFds.data(), _pollFds.size(), -1); // Attendre un event sur nimporte quel socket
		
		if (pollEvents < 0) {
			std::cerr << "poll() error" << std::endl;
			break;
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
		    if (_pollFds[i].revents == 0)
		        continue;
		
		    bool clientRemoved = false;
		
		    if (_pollFds[i].revents & POLLIN)
		    {
		        if (_pollFds[i].fd == _serverSocket)
		            acceptNewClient();
		        else
		        {
		            size_t oldSize = _pollFds.size();
		            handleClientData(_pollFds[i].fd);
				
		            if (_pollFds.size() < oldSize)
		            {
		                clientRemoved = true;
		                i--;
		            }
		        }
		    }		
		    if (!clientRemoved && (_pollFds[i].revents & (POLLERR | POLLHUP)))
		    {
		        removeClient(_pollFds[i].fd);
		        i--;
		    }
		}
	}
}



bool Server::doesChannelExist(const std::string& channelName)
{
	return (_channels.find(channelName) != _channels.end());
}


bool Server::isValidChannelName(const std::string& channelName) const
{
    if (channelName.empty())
        return (false);
    
    if (channelName[0] != '#' && channelName[0] != '&')
        return (false);
    
    return (true);
}

Channel*	Server::findChannel(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator i = _channels.find(channelName);

	if (i != _channels.end())
		return (i->second);
	return (NULL);
}

Channel*	Server::createChannel(const std::string& channelName)
{
	if (!isValidChannelName(channelName))
	{
		std::cerr << "Invalid channel name: " << channelName << std::endl;
		return (NULL);
	}

	if (doesChannelExist(channelName))
	{
		std::cerr << "Channel " << channelName << " already exists" << std::endl;
		return (findChannel(channelName));
	}

	Channel* newChannel = new Channel(channelName);
	_channels[channelName] = newChannel;
	std::cout << "Channel " << channelName << " created" << std::endl;

	return (newChannel);
}

Channel*	Server::getOrCreateChannel(const std::string& channelName)
{
	Channel* channel = findChannel(channelName);

	if (channel != NULL)
        return (channel);

	return (createChannel(channelName));
}

Channel* Server::getChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

void Server::deleteChannelIfEmpty(const std::string& channelName)
{
	Channel* channel = findChannel(channelName);

	if (channel == NULL)
		return;

	if (!channel->isEmpty())
		return;
    
	std::cout << "Deleting empty channel " << channelName << std::endl;
	delete channel;
	_channels.erase(channelName);
}

