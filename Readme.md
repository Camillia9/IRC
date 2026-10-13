*This project has been created as part of the 42 curriculum by camansou, sachanai*

# ft_irc - IRC Server Implementation

## Description

A fully functional IRC (Internet Relay Chat) server written in C++98 that supports 
multiple simultaneous clients, channel management, operator privileges, and file 
transfers. The project includes an automated bot as a bonus feature.

## Instructions

### Compilation
```bash
make          # Compile the server
make bonus    # Compile the bot
```

### Execution

**Server:**
```bash
./ircserv <port> <password>

# Example:
./ircserv 6667 mdp
```

**Bot:**
```bash
cd bonus
./ircbot <host> <port> <password> <nickname>

# Example:
./ircbot localhost 6667 mdp TBot
```

### Connecting with IRC Clients

**Using irssi:**
```bash
irssi
/connect localhost 6667 mdp nick
```

**Using netcat:**
```bash
nc localhost 6667
PASS mdp
NICK user
USER user 0 * :User
JOIN #general
```

## Features

### Mandatory Features
- Multi-client support using poll()
- User authentication (PASS, NICK, USER)
- Channel operations (JOIN, PART, TOPIC)
- Operator management (automatic transfer on disconnect)
- Private and channel messaging (PRIVMSG)
- Channel modes: +i (invite-only), +t (topic restriction), +k (password), +o (operator), +l (user limit)
- KICK and INVITE commands

### Bonus Features
- **IRC Bot**: Automated bot with multiple commands
  - !help - List available commands
  - !time - Current server time
  - !info - IRC commands reference
  - And more...
- **File Transfer**: DCC (Direct Client-to-Client) support for file transfers between clients

## Testing

Automated test scripts :
```bash
python3 tests/test_join_part.py      # 50 JOIN/PART cycles
python3 tests/test_bot_cmd.py   # Bot command testing
python3 tests/test_multi_clients.py  # 10 simultaneous clients
python3 tests/test_stress.py         # Stress test
```

## Resources

### Documentation
- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [poll() manual](https://man7.org/linux/man-pages/man2/poll.2.html)

### AI Usage
Claude (Anthropic) was used as a learning and debugging assistant for:
- Understanding IRC protocol specifications (RFC 1459)
- Learning C++98 STL usage and best practices
- Understanding network programming concepts (non-blocking I/O, poll(), signals)

All implementation was done by the student. AI served as an educational tool, 
similar to consulting documentation, Stack Overflow, or asking a mentor for guidance.

## Authors

- camansou (https://github.com/Camillia9)
- sachanai (https://github.com/Sarahchanai)