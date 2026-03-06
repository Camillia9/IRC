#include "Channel.hpp"

Channel::Channel(const std::string& name)
{
    _name = name;
    _topic = "";
	_topicRestricted = false;
	_inviteOnly = false;
	_key = "";
	_userLimit = 0;
}

Channel::~Channel() {}

const std::string& Channel::getName() const
{
    return _name;
}


void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

bool Channel::isEmpty() const //utile pr deletechannelifempty
{
    return _members.empty();
}

const std::map<std::string, Client*>& Channel::getMembers() const //pour cam i think
{
	return _members;
}


bool Channel::isInviteOnly()const
{
	return _inviteOnly;
}

bool Channel::isTopicRestricted()const
{
	return _topicRestricted;
}

bool Channel::isInvited(const std::string &nick)const
{
	return _invitedUsers.find(nick) != _invitedUsers.end();
}

std::string Channel::getKey()const
{
	return _key;
}

int Channel::getUserLimit()const 
{
	return _userLimit;
}

void Channel::setTopicRestricted(bool value)
{
	_topicRestricted = value;
}

void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
}

void Channel::setKey(const std::string &key)
{
	_key = key;
}

void Channel::setUserLimit(int limit)
{
	_userLimit = limit;
}

void Channel::addInvited(const std::string &nick)
{
	_invitedUsers.insert(nick);
}

void Channel::removeInvited(const std::string &nick)
{
	_invitedUsers.erase(nick);
}





bool Channel::isMember(const std::string& nickname) const
{
    if (_members.find(nickname) != _members.end())
        return true;
    return false;
}

void Channel::addMember(Client* client)
{
    std::string nickname;

    if (client == NULL)
        return;

    nickname = client->getNickname();

    if (_members.find(nickname) != _members.end())
	//if (isMember(nickname))
        return;

    bool firstMember = _members.empty(); //verif si prems mb
	
	_members[nickname] = client; //add ds la map

    std::cout << "Client " << nickname << " joined channel " << _name << std::endl;

    if (firstMember) //si prems membre -> opérateur auto
    {
        _operators[nickname] = client;

        std::cout << "Client " << nickname << " is now operator of channel " << _name << std::endl;
    }
}

void	Channel::removeMember(Client* client)
{
	if (client == NULL)
		return;

	std::string nickname = client->getNickname();

	_members.erase(nickname);
	_operators.erase(nickname);
    
	std::cout << nickname << " left " << _name << std::endl;

}













bool Channel::isOperator(const std::string& nickname) const
{
    if (_operators.find(nickname) != _operators.end())
        return true;
    return false;
}

void	Channel:: addOperator(Client* client)
{
	if (client == NULL)
		return;
	
	std::string nickname = client->getNickname();

	if (_members.find(nickname) == _members.end()) //verif si membre avant tt
	//if (!isMember(nickname))
	{
		std::cerr << "Error: " << nickname << " is not a member of " << _name << std::endl;
        return;
	}

	if (_operators.find(nickname) != _operators.end()) //verif si ps déjà opé qd même
	//if (isOperator(nickname))
	{
		//std::cerr << "Error: " << nickname << " is already an ope of " << _name << std::endl;
        return;
	}

	_operators[nickname] = client;

    std::cout << nickname << " is now operator of channel " << _name << std::endl;
}

void	Channel::removeOperator(Client* client)
{
	if (client == NULL)
		return;
	
	std::string nickname = client->getNickname();

	if (_operators.find(nickname) == _operators.end())
	//if (!isOperator(nickname))
		return;

	_operators.erase(nickname);

	std::cout << nickname << " is no longer operator of " << _name << std::endl;
}

//si ds mon channel y'a moi camillia et lewis bah cette fonc° doit send mon mess "citron" à camillia et lewis MAIS sans me l'envoyer à moi même (logique)
//utilisa° de std::map pour parcourir les diff clients
//map = tab de 2 colonnes

void	Channel::broadcast(const std::string& message, Client* sender) //à revoir pour bien comprenedre internement le process w/ schema
{

	std::map<std::string, Client*>::const_iterator i = _members.begin(); //mon curseur pointe sur le prems client

	for (i = _members.begin(); i != _members.end(); ++i)
	{

        Client* clientDuChannel = i->second; //pr récup le nom du client et sa valeur (son mess)

        if (clientDuChannel != sender)
        {
            int numeroSocket = clientDuChannel->getFd();

            int result = send(numeroSocket, message.c_str(), message.length(), 0);

			if (result < 0)
				std::cerr << "Error: fail to send to "  << clientDuChannel->getNickname() << std::endl;
        }
    }
}



std::string Channel::getMembersList() const
{
	std::string membersList;
    
	std::map<std::string, Client*>::const_iterator i;


	for (i = _members.begin(); i != _members.end(); i++)
	{
		if (!membersList.empty())
			membersList += " ";

		const std::string& nickname = i->first;

		if (isOperator(nickname))
			membersList += "@";
 
		membersList += nickname;
	}

	return membersList;
}



