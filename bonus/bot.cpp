#include "bot.hpp"

extern bool g_running;

Bot::Bot(const std::string &nickname) : _nick(nickname), _realName("BOT Serv"), _recvBuffer("") {
	srand(time(0));
}

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
	fcntl(_socket, F_SETFL, O_NONBLOCK);

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

std::string Bot::receiveMessage()
{
    char buffer[512];

    while (true)
    {
        int bytes_read = recv(_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            _recvBuffer += buffer;
        }
        else if (bytes_read == 0)
        {
            std::cerr << "Server disconnected" << std::endl;
            close(_socket);
            return "";
        }
        else // bytes_read == -1
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; // ✅ PLUS RIEN À LIRE → normal

            std::cerr << "recv() error:" << std::endl;
            close(_socket);
            return "";
        }
    }
    size_t pos = _recvBuffer.find("\r\n");
    if (pos == std::string::npos)
        return "";

    std::string line = _recvBuffer.substr(0, pos);
    _recvBuffer.erase(0, pos + 2);

    return line;
}

void Bot::joinChannel(const std::string &channel)
{
	sendMessage("JOIN " + channel + "\r\n");
	if (channel.empty())
		return;

	std::cout << "OK: Bot joined channel " << channel << std::endl;
}

void Bot::handleMessage(const std::string &line)
{
	if (line.find("PRIVMSG") != std::string::npos) 
	{
		std::istringstream iss(line);
		std::string id;
		std::string command;
		std::string target;
		std::string message;

		iss >> id >> command >> target;

		getline(iss, message);

		if (!message.empty() && message[0] == ' ')
			message.erase(0, 1);
		if (!message.empty() && message[0] == ':')
			message.erase(0, 1);

		std::string nick = id.substr(1, id.find('!') - 1);

		std::cout << "From: " << nick << " | To: " << target << " | Msg: " << message << std::endl;

		if (!message.empty() && message[0] == '!')
			handleCommand(nick, target, message);
	}
}

void Bot::handleCommand(const std::string &from, const std::string &target, const std::string &message)
{
	std::istringstream iss(message.substr(1));
	std::string cmd;
	iss >> cmd;

	std::string replyTo = target;
	if (target == _nick)
		replyTo = from;

	if (cmd == "help")
		cmdHelp(replyTo);
	else if (cmd == "time")
		cmdTime(replyTo);
	else if (cmd == "ping")
		cmdPing(replyTo, from);
	//else if (cmd == "info")
	//	cmdInfo(replyTo);
}

void Bot::sendMultilineMessage(const std::string &target, const std::vector<std::string> &lines)
{
    for (size_t i = 0; i < lines.size(); i++)
    {
        sendMessage("PRIVMSG " + target + " :" + lines[i] + "\r\n");
        usleep(100000);  // Pause 100ms entre chaque message (évite le flood)
    }
}

void Bot::run()
{
	std::cout << "Bot is running... Press Ctrl+c to stop" << std::endl;

	struct pollfd pfd;
	pfd.fd = _socket;
	pfd.events = POLLIN;

	while (g_running) 
	{
		int pollEvents = poll(&pfd, 1, -1);  // -1 bloque jusqu'a event

		if (pollEvents == -1) {
			std::cerr << "poll error" << std::endl;
			break;
		}

		if (pfd.revents & POLLIN) {
			while (true) {
				std::string msg = receiveMessage();
				if (msg.empty())
					break;
			
				// DEBUG
				std::cout << "<< " << msg << std::endl;
				handleMessage(msg);
			}
		}
		if (pfd.revents & (POLLHUP | POLLERR)) {
			std::cerr << "Connection closed by server" << std::endl;
			break;
		}
	}
	close (_socket);
}
