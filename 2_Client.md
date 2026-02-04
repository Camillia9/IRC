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