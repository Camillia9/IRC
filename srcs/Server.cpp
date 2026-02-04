#include "Server.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _serverSocket(-1)
{
	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
}

Server::~Server()
{
	if (_serverSocket != -1)
		close(_serverSocket);
}

void Server::createSocket()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
		throw std::runtime_error("Failed to create socket");
	std::cout << "OK, Socket created" << std::endl;
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

void Server::acceptConnections()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	std::cout << "...Waiting for a connection..." << std::endl;

	int clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientSocket < 0)
		throw std::runtime_error("Failed to accept connection");

	std::cout << "OK, Client connected!" << std::endl;
	
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));

	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead > 0)
	{
		std::cout << "Received: " << buffer << std::endl;

		const char *response = "Echo: ";
		send(clientSocket, response, strlen(response), 0);
		send(clientSocket, buffer, bytesRead, 0);
	}
	close(clientSocket);
	std::cout << "Client disconnected" << std::endl;
}

void Server::start()
{
	createSocket();
	bindSocket();
	listenForConnections();
	acceptConnections();
}