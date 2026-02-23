# C'est quoi une socket ?

Une **socket** est un point de communication réseau, ou, un canal de communication entre deux machines (ou deux programmes).
 Imagine une prise électrique :
- Tu branches un appareil (client)
- Dans une prise murale (serveur)
- L'électricité passe (les données)

Côté serveur IRC :

tu ouvres une prise (socket)
tu la branches sur un port (bind)
tu écoutes (listen)
tu acceptes des appels (accept)
tu échanges des messages (recv / send)
👉 La socket est un fichier spécial (file descriptor).
_serverSocket est juste un int.

# Les étapes pour créer un serveur TCP

1. socket()     → Créer une socket
2. bind()       → Attacher la socket à une adresse:port
3. listen()     → Mettre la socket en mode écoute
4. accept()     → Accepter une connexion (bloquant)
5. recv()       → Recevoir des données
6. send()       → Envoyer des données
7. close()      → Fermer la connexion


# 🎯Le Rôle de la Classe Server
La classe Server est le cœur de ton application. Elle doit :

1. Gérer la socket serveur (créer, configurer, écouter)
2. Accepter des connexions de clients
3. Recevoir/Envoyer des messages
4. Plus tard : gérer plusieurs clients, les commandes IRC, etc.

**Explications Server.hpp**
# int _serverSocket`
- C'est un *file descriptor* (FD)
- Un FD est juste un nombre entier qui identifie une ressource (fichier, socket, etc.)
- Quand tu fais `socket()`, le système te donne un numéro (ex: 3, 4, 5...)
- Tu utilises ce numéro pour toutes les opérations sur cette socket

# struct sockaddr_in _serverAddr
- C'est une structure qui contient l'adresse réseau complète. Elle contient les infos nécessaires pour que le système sache où "brancher" ta socket.
 - Elle contient :
  - Le type d'adresse (IPv4 ou IPv6)
  - L'adresse IP (ici INADDR_ANY = toutes les interfaces)
  - Le port

*EXEMPLE de ce qu'elle contient*
sin_family = AF_INET        // IPv4
sin_addr   = 0.0.0.0        // Toutes les interfaces réseau de la machine
sin_port   = 6667           // Le port

**Explications Server.cpp**

# La variable global :
  Pour l'nstant dans le cpp, extern pour dire "Cette var global est utilise quelque part d'autre (main)". Elle sert a capter le ^C. Des qu'il a etait intercepte on sort de la boucle et le destructeur est appelle. Tout est bien ferme.

# Le Constructeur :
- Le _port et le _password seront ceux passe en parametres.
- _ServeurSocket initialise a -1 pour dire "pas encore cree".
- Mise a 0 de la strcuture _serverAddr pour eviter valeurs aleatoires.

# Le Destructeur :
- Pour touts les clients cree, on close la socket de chaque clients puis on le delete. 
- Si la socket a ete cree (!= -1), on la ferme proprement.
--> Ca libere les ressources systemes
--> Ca evite que le port reste "occupe" et ne puisse pas relancer le serveur.

# CreateSocket() :
La Signature de la fonction:
    *int socket(int domain, int type, int protocol);*
domain: la "famille" reseau (IPv4); 
type: le type de communicaion (TCP);
protocol: “Choisis le protocole par défaut pour ce domaine + type”
AF_INET + SOCK_STREAM → TCP

- Les Arguemts : socket(AF_INET, SOCK_STREAN, 0) 
   --> AF_INET: On veut utiliser IPv4. *(127.0.0.1, 192.168.x.x)*
   --> SOCK_STREAM: On veut du TCP *(fiable, ordonné, connecté)*
   --> 0: Protocole par defaut (TPC)
- Le retour:
  --> Si succes retourne un nombre positif (le FD, ex 3).
  --> Si erreur: retourne -1

  # bindSocket() :
- Etape 1: Configurer l'adresse:

    _serverAddr.sin_family = AF_INET; // On dit qu'on utilise IPv4

    _serverAddr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = 0.0.0.0 = "écoute sur TOUTES les interfaces réseau"

    _serverAddr.sin_port = htons(_port); //htons = Host TO Network Short. htons() convertit le port dans le bon format, Exemple : htons(6667) → 0x1A0B

- Etape 2: Option SO_REUSEADOR
  La signature : 
    *setsockopt(fd, level, option, value, size)*
    VISUALISATON GLOBAL : Quand tu appelles setsockopt, tu dis au kernel :
    “À tel niveau (level), je veux régler telle option (option) avec telle valeur (value) de telle taille (size).”
    "Sur cette socket, au niveau général, active l’option SO_REUSEADDR en lui donnant la valeur 1, stockée sur sizeof(int) octets.”

    - fd : La socket a configurer. Ici _serverSocket.
    - level : "A quel niveau on souhaite appliquer l'option (arguments 3)". Ici SOL_SOCKET = options qui concernet la socket elle-meme, independemment du protocole(TCP/IP). On l'utilise car SO_REUSEADDR n'est pas une option TCP, elle est stockee au niveau socket.
    - option : dit quelle option précise tu veux modifier dans le niveau donné. DONC : “Au niveau socket générale, je veux modifier l’option SO_REUSEADDR”.
    SO_REUSEADDR : autorise la reutilisation du port. SI on lance le serveur avec le port 6667, qu'on CTRL^C et qu'on relance, le kernel garde le port en TIME_WAIT (securite TCP); grace a SO_REUSEADDR on peux reutiliser le port juste apres.
    - value : pointeur vers la donnée que tu veux donner à l’option. Ici 1 --> active. Sinon 0 --> descative
    - size : copie size octets depuis l’adresse value. Ici sizeof(opt) = 4 octets.

- Etape 3: Bind
La signature : 
    *bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)*
    VISUALISATION GLOBAL : “Cette socket doit utiliser CETTE adresse réseau précise.”
    Concretement : “Associe cette socket à l’adresse IPv4 décrite par _serverAddr (port + IP), en lisant exactement la taille de cette structure.”

  bind() :
      - Associe la socket au port spécifié
      - "Réserve" le port pour ton serveur
      - Après ça, aucun autre programme ne peut utiliser ce port

   - sockfd : _serverSocket : C’est la socket que tu veux attacher à une adresse. Grace a bind() ce fd devient associé à un port + IP
   - const struct sockaddr *addr : L'adresse a laquelle la socket est attachee. bind() doit pouvoir fonctionner avec IPv4 IPv6 etc. Donc elle prend : "const struct sockaddr *". Mais nous on travail en IPv4 (sockaddr_in), c'est pourquoi on cast: (struct sockaddr *)&_serverAddr. Maintenant le port et l'IP sont bien interprete.
   - socklen_t addrlen : Combien d'octets lire. sizeof(_serverAddr) : L'adresse commence a 'addr' et fait X octets. Concretement : copie addrlen octets depuis addr.


  # listenForConnections() :

    *listen(int socketfd, int backlog);*
       --> “Transforme cette socket en socket serveur et autorise jusqu’à 10 connexions clientes à attendre avant d’être acceptées.”

      - socketfd : Sur QUOI on ecoute ? Ici sur _serverSocket. 
        ⚠️ SEULE une socket bindee peut ecouter
        Elle sert UNIQUEMENT a :
          - recevoir des demandes de connexion
          - les mettre en attente
      - backlog : Combien de connexions en attente ? Ici 10.
        --> La taille de la file d’attente des connexions entrantes.

  # initPoll() :
    
    On cree une structure pollfd qui dit : 
      - Ce fd correspond a _serverSocket (celui qui fait listen).
      - Je veux etre notifie lorsqu'un client essaie de se connecter (POLLIN)

      - Puis on ajoute ce serverSocket dans le tabeau que poll() surveillera

      En gros Cette fonction Dit à poll() :
    “Surveille mon socket serveur et préviens-moi quand un client essaie de se connecter.”

  # acceptNewClient() :

    Deja on prepare une structure pour recuperer les infos du clients (Port, IP) :
    Ex pour  struct sockaddr_in clientAddr;
          clientAddr.sin_family  // AF_INET
          clientAddr.sin_port    // port du client
          clientAddr.sin_addr    // IP du client

      
    Partie 1: accetpt()
    *int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);*
      --> On accepte la connexion.
        Ce que ça fait :
      - Retire la connexion en attente dans la file listen()
      - Crée une nouvelle socket dédiée à ce client
      - Retourne son fd
  Important :
  -> _serverSocket reste le socket principal
  -> clientSocket est une nouvelle socket spécifique à ce client
          
      - socketfd : Sur QUELLE socket tu acceptes la connexions. Ici _serverSocket est la socket d'ecoute sur laquelle on a fait listent().
      - struct sockaddr &Clientaddr : OÙ le kernel ecrit l'adresse du client. On donne au kernel une zone memoire ou on peux ecrire l'adresse du client.
      - socklen_t &clientLen : C'est un parametre IN/OUT (entree, sortie). Pour avoir la taille reel du buffer utilise
      
    Partie 2: fcntl()
    *fcntl(clientSocket, F_SETFL, O_NONBLOCK);*
      TRES IMPORTANT. Pour mettre la socket du client en non bloquante

  # addClient() :
    On cree un nouvel objet Client associe a ce fd (qui vient d'un accept).
    On l'ajoute a la liste de tout les clients que le Server connait.
    On crée une structure pollfd pour ce client.
      fd = fd → on surveille ce client
      events = POLLIN → on veut savoir quand il envoie un message
      revents = 0 → initialisation propre
    On ajoute ce client dans la liste surveille par poll(). 

  # removeClient() :
    --> On supprime un client du serveur
    --> On libère sa mémoire
    --> On l’enlève de poll()
    --> On ferme sa socket

  # getClientByFd() 
    --> Rechercher un client grace a son FD.
  
  # handleClientData()
    --> On lit les données envoyées par un client
    --> On les ajoute à son buffer interne
    --> On extrait les messages complets (\r\n)
    --> On affiche chaque message
    --> Si le client est parti → on le supprime

  # run() :
  
    On lance toutle processus. 
    *Flux Complet d'Exécution*

Voici ce qui se passe quand tu lances `./ircserv 6667 pass` :
run()
  ↓
  ├─ createSocket()
  │     → socket() retourne FD 3
  │
  ├─ bindSocket()
  │     → bind(3, "0.0.0.0:6667")
  │
  ├─ listenForConnections()
  │     → listen(3, backlog)
  │
  ├─ initPoll()
  │     → ajoute FD 3 dans _pollFds
  │     → events = POLLIN
  │
  └─ Affiche :
        "Server running... Press Ctrl+C to stop"
                   ┌───────────────┐
                   │   poll()      │
                   └───────┬───────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
          Nouvelle      Données     Déconnexion
          connexion     client      client
              │            │            │
        acceptNewClient  handleData  removeClient



# RESUME Concep Cles :
| Concept                 | Explication                                                    |
|-------------------------|----------------------------------------------------------------|
| File Descriptor (FD)    | Entier qui identifie une socket (ou un fichier)                |
| sockaddr_in             | Structure contenant l’adresse IP et le port                    |
| socket()                | Crée une socket et retourne un file descriptor                 |
| bind()                  | Associe la socket à une adresse IP et un port                  |
| listen()                | Met la socket en mode écoute (serveur)                         |
| accept()                | Attend et accepte une connexion cliente (bloquant par défaut)  |
| recv()                  | Lit les données envoyées par le client                         |
| send()                  | Envoie des données au client                                   |
| close()                 | Ferme une socket et libère le file descriptor                  |
