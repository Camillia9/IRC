🧠 Projet IRC – Roadmap Technique
🔵 CHLOÉ — Infrastructure Réseau & Gestion Bas Niveau
Phase 1 — Classes de base

⏱️ Durée estimée : 1 jour
🟢 À faire en PREMIER (indépendant)

🎯 Objectif

Poser les fondations de l’architecture serveur.

1️⃣ Classe Server

Attributs :
int server_fd
int port
std::string password
fd_set master_set
int max_fd

Méthodes :
Constructeur
Destructeur

2️⃣ Classe Client

Attributs :
int socket_fd
std::string nickname
std::string username
bool authenticated
bool registered
std::string buffer (buffer de réception)

Méthodes :
Getters / Setters

3️⃣ Classe Channel (structure uniquement)

Laurent complètera la logique métier plus tard.

📌 Pourquoi commencer ici ?

Laurent a besoin de la structure Client pour développer son parser.

Ces classes sont la base du projet.

Permet d’aligner l’architecture commune.

✅ Livrable

Fichiers .hpp validés ensemble.

Phase 2 — Architecture select() & Gestion des buffers

⏱️ 3–4 jours
🟡 PRIORITÉ ABSOLUE

2.1 Initialisation serveur (1 jour)
socket(AF_INET, SOCK_STREAM, 0);
setsockopt();
bind();
listen();

Queue de 10 connexions

Initialisation du fd_set master
Ajout du server_fd

2.2 Boucle principale avec select() (1 jour)

À chaque itération :
Copier master_set → read_set

Appeler :
select(max_fd + 1, &read_set, NULL, NULL, NULL);

Parcourir tous les FDs
Tester avec FD_ISSET()

Distinguer :
Nouvelle connexion
Données client existant

2.3 Acceptation des connexions (0.5 jour)

Quand FD_ISSET(server_fd) :
accept()
Création d’un Client
FD_SET(new_fd)
Mise à jour max_fd
Ajout dans map/list des clients

2.4 🔴 Buffer circulaire & extraction des lignes (CRITIQUE)

⏱️ 1–1.5 jours

Problèmes à gérer
Un recv() peut :

Contenir plusieurs messages
Contenir un message incomplet

IRC limite : 512 caractères (incluant \r\n)

Implémentation

Chaque Client possède :
std::string buffer;

Lors d’un recv() :
Ajouter au buffer :
client.buffer += temp;
Chercher \r\n
Extraire la ligne complète
La retirer du buffer

Appeler :
processCommand(Client* client, std::string line);

Continuer tant qu’il y a \r\n

⚠️ Si buffer > 512 → erreur + déconnexion

Pourquoi critique ?

Sans cette gestion :
Messages coupés
Messages fusionnés
Serveur inutilisable

Phase 3 — Authentification

⏱️ 1–2 jours

⚠️ Dépend de Phase 2 + Parser de Laurent

3.1 États du client
Dans Client :

bool password_ok;
bool has_nickname;
bool registered;

Créer :
std::map<std::string, Client*> nicknames;

3.2 PASS
Doit être première commande
Vérifier mot de passe
Si incorrect → erreur 464 + déconnexion
Sinon → password_ok = true

3.3 NICK
Validation :
≤ 9 caractères
Commence par lettre
Caractères autorisés : [A-Za-z0-9_\-\[\]]
Unicité
Erreur si déjà pris → 433

3.4 USER
Format :
USER <username> <hostname> <servername> :<realname>
Stocker :
username
realname

3.5 Messages de bienvenue

Quand :
password_ok && has_nickname && username != ""

Envoyer :

001 RPL_WELCOME

002 RPL_YOURHOST

003 RPL_CREATED

004 RPL_MYINFO

Phase 4 — Gestion des déconnexions
⏱️ 0.5 jour

Détection
recv() <= 0
QUIT
Même procédure que déconnexion réseau.
Nettoyage global

Fonction :
void removeClientFromAllChannels(Client* client);

Étapes :
Retirer de tous les channels
Notifier les membres
Supprimer nickname
Retirer fd
close(fd)
delete client

Phase 5 — Codes IRC (Helpers)

⏱️ 0.5 jour
🟢 Parallèle

Fonction utilitaire :
sendNumericReply(Client*, int code, std::string message);

Format :
:servername CODE nickname :message\r\n

🟢 LAURENT — Parsing & Logique Métier
Phase 1 — MessageParser

⏱️ 3–4 jours
🟡 PRIORITÉ ABSOLUE

Format IRC
[:prefix] COMMAND param1 param2 :last param

Exemple :

PRIVMSG #chan :Hello world

Structure ParsedMessage
struct ParsedMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

Parsing

Étapes :
Extraire prefix si présent
Extraire commande
Extraire paramètres
Gérer : pour dernier param
Limite IRC : max 15 paramètres.
Dispatcher
Option simple :
if (msg.command == "JOIN") handleJoin();

Option propre :
std::map<std::string, HandlerFunction>

Erreur inconnue → 421

Phase 2 — Channels
⏱️ 2 jours

Classe Channel
Attributs :
name
members
operators
topic
modes
password
user_limit
invited

Méthodes :
addMember()
removeMember()
isMember()
broadcast()

Phase 3 — Commandes Channels
⏱️ 3–4 jours

JOIN
Vérifier modes : +i, +k, +l
Créer channel si inexistant

Envoyer :
JOIN broadcast
332 ou 331
353
366

PART
Vérifications
Broadcast
Supprimer si vide

Phase 4 — PRIVMSG & NOTICE
⏱️ 2 jours

Vers Channel
Vérifier membership
Broadcast sauf expéditeur

Vers User
Vérifier existence
Envoyer message direct

Phase 5 — Commandes opérateurs
⏱️ 2 jours

KICK

INVITE

TOPIC

Phase 6 — MODE 🔴
⏱️ 4–6 jours
⚠️ Plus complexe

Modes à implémenter
Mode	Description
+i	Invite-only
+t	Topic protégé
+k	Mot de passe
+o	Operator
+l	Limite users
Parser MODE

Exemple :
MODE #chan +oki-l Alice secret
Parser caractère par caractère.
Broadcast changements

Format :
:nick!user@host MODE #channel +o Alice

Query MODE

Si :

MODE #channel

Envoyer :

324 RPL_CHANNELMODEIS

🎯 Résumé stratégique

Ordre recommandé :

Infrastructure réseau solide
Buffer handling parfait
Parser fiable
Channels
Privmsg

MODE en dernier