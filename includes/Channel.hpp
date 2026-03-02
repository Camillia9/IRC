#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <sys/socket.h>

#include "Client.hpp"



class Channel
{
	public :
				Channel(const std::string& nom);
				~Channel();


				const std::string&	getName() const; //fait
				const std::map<std::string, Client*>& getMembers() const; //pr cam i think
				const std::string& getTopic() const; //fait

				void	setTopic(const std::string& topic); //fait

				void	addMember(Client* client); //fait
				void	removeMember(Client* client); //fait
				bool	isMember(const std::string& nick) const; //fait

				void	addOperator(Client* client); // à faire
				bool	isOperator(const std::string& nick) const; // fait
				void	removeOperator(Client* client); // à faire 

				void	broadcast(const std::string& message, Client* sender = NULL); // done
			
				std::string	getMembersList() const; //pour Cam son utilité ça je sais pas

				bool	isEmpty() const; //fait






	private :


				std::string _name;
				std::string _topic;

				std::map<std::string, Client*> _members;
				std::map<std::string, Client*> _operators;



};

//nb removeifempty




#endif