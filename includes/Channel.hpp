#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <sys/socket.h>
#include <set>

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

				// Modes
				bool        isTopicRestricted() const; // Est-ce que le mode +t est actif ?
				bool        isInviteOnly() const; // est-ce que le mode +i est actif ?
				bool        isInvited(const std::string &nick) const; // Est-il dans la liste des invites ? 
				std::string getKey() const; // recupere le mdp du channel
				int         getUserLimit() const; // recupere la limite du nb du channel

				void setTopicRestricted(bool value);
				void setInviteOnly(bool value);
				void setKey(const std::string &key);
				void setUserLimit(int limit);
				void addInvited(const std::string &nick);
				void removeInvited(const std::string &nick);

				std::string getModesString()const; // Cree la string affichant les modes du server






	private :


				std::string _name;
				std::string _topic;

				std::map<std::string, Client*> _members;
				std::map<std::string, Client*> _operators;

				// MODES
				bool _topicRestricted; //t
				bool _inviteOnly; // i
				std::set<std::string>_invitedUsers; // liste des invites
				std::string _key; // k
				int _userLimit; // l



};

//nb removeifempty




#endif