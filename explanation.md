Le projet consiste à développer un serveur Internet Relay Chat (Discussion Relayée par Internet) minimaliste en C++

Protocole de communication textuelle à temps réel sur Internet +-= à un forum qui fournit de manière simple rapide et efficiente 

+ : polyvalent et favorise comm instantannée accessible ordi ou tel, simple et compatibles avec ts les appareils 
    pro et perso

- moins moderne


Les clients communiquent par le biais du serveur qui traite et gère le textuel



IRC = protocole en mode texte dont le client le + simple est n'importe quel programme TCP capable de se connecter à un serveur
      Le but principal du protocole IRC = fournir une base afin que des clients puissent communiquer entre eux. 


+- = convention transfert de data sur internet
request -> replies

Objectifs :
            - Comprendre le modèle serveur - client
            - Comprendre la gestion des sokets
            - Comprendre la communication en réseau
            - Comprendre la gestion de connexions multiples 








                          1--\
                              A        D---4
                          2--/ \      /
                                B----C
                               /      \
                              3        E

   Servers: A, B, C, D, E         Clients: 1, 2, 3, 4


Un mess btw le client 1 et le client 2 est seulement vu par le serveur A, qui l'envoie direct au client 2.

Un mess btw le client et le client 3 est vu uniquement par les serveurs A ET B et par le client 3. No other clients or servers are allowed to see the message.

Un mess btw le client 2 et le client 4 est uniquement vu par les serveurs A, B, C, D et le client 4.









./ircserv <port> <password>






                                    SERVEUR (fournisseur de service)

- Colonne vertébrale de l'IRC

- Fournit un point auquel les clients peuvent se connecter pour parler entre eux & un point auquel les autres serveurs peuvent se connecter -> formant le réseau IRC








                                    CLIENT (demandeur de service)

- Tout ce qui se connecte à un serveur (& qui n'est ps un serveur)

- Chq client est différencié des autres clients par un pseudonyme unique (long max 9 char)

- Tous les serveurs doivent connaitre ces infos sur le client : - pseudo
                                                                - vrai nom de l'hôte (sur lequel le client est executé)
                                                                - nom d'utilisateur du client sur cet hôte
                                                                - serveur auquel il est co








                            
                OPÉRATEUR

-> Catégorie de clients spéciale

- Execute des fonctions de maintenance générale sur le réseau

TÂCHES :- Déconnexion/Reconnexion de serveurs (pr prévenir à long terme les pb de mauvais routage réseau)
        
        - Évincer un user co au réseau = clore une co btw un serveur et un client 









                                        CANAL (=SALON)


DEF : Grp nommé d'1 ou plusieurs clients qui recevront ts les mess adressés à ce canal 

- Créé qd le 1er client y accède 
- Disparait lorsque le dernier le quitte

Nom de canal = str (commençant pr un char '&' ou '#') d'une longueur max de 200 char. En dehors du fait que le prems char doive être un '&' ou un '#', la seule restriction sur le nom d'un canal est qu'il ne peut ps contenir d'espace (' '), de contrôle G (^G ou ASCII 7), ou de virgule (',' qui est utilisée comme séparateur de liste dans le protocole).


COMPO :

		- son nom 
		- ses membres
		- ses opérateurs (cf CLIENT / Opérateur) (= admin)
		- ses modes ()


PRIVMSG et NOTICE -> seuls mess dispo qui réalisent effectivement l'acheminement d'un message textuel d'un client à un autre - le reste le rend juste possible & assure que cela se passe de façon fiable et structurée.



Une commande IRC est toujours définie par le caractère « / » placée en début de ligne. (ex : /JOIN #canal)







                                    UNE SOKET C'EST QUOI ?

-> point de com

La combinaison  d'une adresse IP et d'un numéro de port source ou 
                d'une adresse IP et d'un numéro de port destination

La soket va fournir une interface (à partir d'un hôte) pour utiliser faciliment les protocoles de transport (ici TCP)

En gros la soket = extrémité d'un canal de com bidirectionnel côté client ou serveur en l'associant à un port














                                        ASPECT TECHNIQUE

Qd un client se connecte à un serveur -> ils s'envoient un flux d'octets -> mess séparés par \n \r



                                        FORMAT DES MESS
                        

Une ligne 








                                LE PARSING DANS IRC CONCRETEMENT ?

ATTENTION limite de params cf RFC -> 15

-> Récup les infos du client -> convention textuelle


struct MessageClient
{
    std::string id;
    std::string command;
    std::vector<std::string> param;
    std::string brut; //debug
};



MessageClient lire_comm(std::string line);


recup_id();
recup_action();
recup_param();




SI la ligne commence par ':' -> lire l'identifiant

lire le premier mot -> commande

lire les mots suivants -> paramètres

SI un mot commence par ':' -> prendre tt le reste comme 1 seul paramètre

SI ligne vide -> return

SI : JOIN ou join ou Join -> ^m commande -> maybe tt convertir en MAJ pr + 2 facilité

si i >= line.size() -> ligne vide -> return



read_ID() done
read_command()
read_param()
skip_spaces() done
read_word() done 










                                NB

Store all your users in a list / map (as user * if possible)

Don’t do multiserver

Make a Message class (with sender, receiver, payload, command called etc) it will be useful.

To get what command to call, do a map with a string as a key (command name) and a pointer to function (it’s cool)

IRC default port is 6667 (6697 for tls), while submiting your work please use them, it’s annoying to specify it while testing in a defense (when using irssi for example, specifying 6667 every time at the end is boring)

If you have time, do REHASH, it’s really cool to see it working
If you have time and did start the projet well, do STATS

To test ipv6 you can use irssi and add -6 during the /connect

Buffer every output, you only are allowed to do 1 (one) send() per select()

Add MSG_NOSIGNAL as a 4th argument for send, it will prevent your programm from crashing under certain condition

Use wireshark / a custom proxy etc… to inspect communication between your reference server (or your server) and you your client





 
 
 Créer le système de dispatch des commandes : 


 void executeCommand(Server &server, Client &client, const t_command &cmd)
{

	const std::string &name = cmd.command;

	if (name.empty())
		return;

	if (name == "PASS")
		execPass(server, client, cmd);

	else if (name == "NICK")
		execNick(server, client, cmd);

	else if (name == "USER")
		execUser(server, client, cmd);

	else if (name == JOIN)
		blabla;

	else
		autre;
}


à faire : EXECUTE COMMAND : JE METS LE PRTOTYPE DANS SA CLASSE SERVER EN PV  


# 📅 PHASE 4 : Channels et messages (6-7 jours)
🔀 Travail en parallèle puis ensemble
**PERSONNE A : Infrastructure des channels**
Ce qu'elle fait

 Créer la classe Channel
 Attributs : nom, topic, membres, opérateurs, modes
 Méthodes : addMember, removeMember, broadcast
 Ajouter map<string, Channel*> _channels dans Server
 Server::getChannel()
 Server::createChannel()
 Server::deleteChannelIfEmpty()

Livrables

Channel classe complète
Gestion des channels dans Server

Dépendances
❌ Aucune (peut commencer après Intégration 3)


						PHASE 4 : CHANNELS ET MESSAGES 


- Créat° classe Channel
- Gest° ds le serveur
- Opérat° sur les channels


NB deletechannelifempty



					ÉTAPE D'AJOUT DANS LE SERVEUR POUR CONNECTER









