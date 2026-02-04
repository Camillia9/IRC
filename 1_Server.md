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
# Le Constructeur :
- Le _port et le _password seront ceux passe en parametres.
- _ServeurSocket initialise a -1 pour dire "pas encore cree".
- Mise a 0 de la strcuture _serverAddr pour eviter valeurs aleatoires.

# Le Destructeur :
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


  # acceptConnection() :

    Partie 1: accetpt()
    *int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);*
      --> BLOQUE le programme jusqu'à ce qu'un client se connecte
          Quand un client arrive, crée une NOUVELLE socket pour communiquer avec lui
          Retourne le FD de cette nouvelle socket
          
      - socketfd : Sur QUELLE socket tu acceptes la connexions. Ici _serverSocket est la socket d'ecoute sur laquelle on a fait listent().
      - struct sockaddr *addr : OÙ le kernel ecrit l'adresse du client. On donne au kernel une zone memoire ou on peux ecrire l'adresse du client. On remplis ici la structure, exemple: 
          clientAddr.sin_family  // AF_INET
          clientAddr.sin_port    // port du client
          clientAddr.sin_addr    // IP du client
      - socklen_t *addrlen : C'est un parametre IN/OUT (entree, sortie). Systeme : 
        Avant appel : "Voici la taille du buffer que je te donne."
        Apres appel : "Voici la taille reelle que j'ai utilise."

        Apres accetpt() :
          👉 Ce que tu obtiens :
          une NOUVELLE socket
          connectée à UN client
          prête à recv() / send()
          ⚠️ IMPORTANT :
          clientSocket ≠ _serverSocket
          ils ont des rôles différents
      
    Partie 2: recv()
    *ssize_t recv(int sockfd, void *buf, size_t len, int flags);*
      --> Lit les données envoyées par le client
          Stocke ces données dans buffer
          Retourne le nombre d'octets lus
          Si retourne 0 : le client s'est déconnecté
          Si retourne -1 : erreur

      - socketfd : La socket sur laquelle on recois.
      - *buf : Adresse memoire ou les donnees recus seront ecrites
      - len : Nombre max d'octects a lire, Ici sizeof(buffer) - 1 pour laisser de la place pour le '\0'
      - flags : Option de reception. Ici 0. 

    Partie 3: send() 
    *ssize_t send(int sockfd, const void *buf, size_t len, int flags);*
      --> Envoie des données au client
          On envoie d'abord "Echo: "
          Puis on renvoie ce que le client a envoyé

      - socketfd : La socket client qui a ete retorunee par accept().
      - buf : Adresse des donnees a envoyer. Ex: send(clientSocket, "Hello", 5, 0);
      - len: Nombre d'octets a envoyer
      - flags: Option d'envoie. Ici 0.

  En gros : 
  “Donne-moi une nouvelle socket correspondant à un client connecté, et écris son adresse dans clientAddr.”

    recv → le kernel écrit dans ton buffer
    send → le kernel lit depuis ton buffer
    toujours sur une socket connectée
    jamais sur la socket serveur

    Partie 4: close() 
    *close(clientSocket);*
      --> Ferme la connexion avec ce client specifique.
      _serverSocket reste ouverte (mais ici le proramme s'arrete).


  # start() :
  
    On lance toutle processus. 
    *Flux Complet d'Exécution*

Voici ce qui se passe quand tu lances `./ircserv 6667 pass` :
main()
  ↓
Server server(6667, "pass")   // Constructeur
  ↓
server.start()
  ↓
  ├─ createSocket()           → socket() retourne FD 3
  ├─ bindSocket()             → bind(3, "0.0.0.0:6667")
  ├─ listenForConnections()   → listen(3, 10)
  └─ acceptConnection()       → accept(3) [BLOQUE ICI]
       ↓
     [Un client se connecte]
       ↓
     accept() retourne FD 4
       ↓
     recv(4) → lit "Hello"
       ↓
     send(4) → envoie "Echo: Hello"
       ↓
     close(4)



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


# 