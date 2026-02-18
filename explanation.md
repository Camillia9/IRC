Le projet consiste à développer un serveur Internet Relay Chat (Discussion Relayée par Internet) minimaliste en C++

Protocole de communication textuelle à temps réel sur Internet

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





PRIVMSG et NOTICE -> seuls mess dispo qui réalisent effectivement l'acheminement d'un message textuel d'un client à un autre - le reste le rend juste possible & assure que cela se passe de façon fiable et structurée.



Une commande IRC est toujours définie par le caractère « / » placée en début de ligne. (ex : /JOIN #canal)


                                    UNE SOKET C'EST QUOI ?

-> point de com

La combinaison  d'une adresse IP et d'un numéro de port source ou 
                d'une adresse IP et d'un numéro de port destination

La soket va fournir une interface (à partir d'un hôte) pour utiliser faciliment les protocoles de transport (ici TCP)

En gros la soket = extrémité d'un canal de com bidirectionnel côté client ou serveur en l'associant à un port



                                LE PARSING DANS IRC CONCRETEMENT ?



-> Récup les infos du client -> convention textuel
