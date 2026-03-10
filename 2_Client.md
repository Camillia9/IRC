# appendToBuffer(const std::string& data);

--> Stock dans le buffer (_recvBuffer) toutes les nouvelles donnees recus.
Ca affiche le contenu du buffer (utile pour debug).

Exemple:
    Client client(5);                     //fd(5)
    client.appendToBuffer("NICK ali");    // Buffer: "NICK ali"
    client.appendToBuffer("ce\r\n");      // Buffer: "NICK alice\r\n"

# extractMessage

--> Tant qu'on a des message complet dans le buffer (fini par \r\n)
    on stock chaque msg dans le vector
    on efface le message stocke
   Si npos (pas de \r\n trouve), la boucle s'arrete et on retourne le message qui ne contienra probablement rien, mais le server attends la suite (sil ya des message plus tard)

Exemple: 
    recv 1 → "NICK toto\r\nUSER t"
    recv 2 → "oto 0 * :Toto\r\n"
   On a deux message (car deux \r\n) mais ils sont coupe:
     "NICK toto"
     "User toto 0 * :Toto"
Grace a extract message, le vector contiendra et renvoiera bien 2 string complete:
     ["NICK toto", "USER toto 0 * :Toto"]
   Maintenant le server est pret a etre parse (NICK, USER, JOIN, etc.)

# clearBuffer()
--> Utile pour vider le buffer en cas d'erreur ou de deconnexion

# Cours sur poll()
poll() observe une liste de FDs et me dis lesquels sont pretes.
Sans poll(), je serais obligée d'appeler recv() sur chaque client un par un, et si un client n'a rien envoyé, recv() bloque et les autres attendent.

Prototype:
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    --> fds: Tableau de structure pollfd
    --> nfds: Nombre d'fd dans le tableau
    --> timeout: Le temps en ms

struct pollfd {
    int   fd;         // File descriptor à surveiller
    short events;     // Ce qu’on veut surveiller (POLLIN, POLLOUT...)
    short revents;    // Ce qui s’est réellement produit
};

Elements a surveiller:
POLLIN    // Données disponibles en lecture (recv ne bloquera pas)
POLLOUT   // Socket prête pour écriture (send ne bloquera pas)
POLLHUP   // Client déconnecté (Hang Up)
POLLERR   // Erreur sur la socket
POLLNVAL  // FD invalide



# Le BOT

# connect()
--> On cree une socketstandart IPv4 protocole tcp. 
--> On prepare l'adresse du server, mais au lieu d'ecouter partout (comme pour le server) on ecoute seuleemnt l'adresse specifique du server (localhost : innet_addr = 127.0.0.1)
--> On se connect au server (nouveau) avec connect()