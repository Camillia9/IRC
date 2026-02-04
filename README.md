Plan de répartition ft_irc - Version claire et structurée
🎯 Division logique du travail
PERSONNE A : Infrastructure & Réseau (SARAH)

Responsable de tout ce qui touche aux sockets, connexions, et communication réseau
Gère la structure du serveur et la boucle principale

PERSONNE B : Logique métier & Protocole IRC (CAMILLIA)

Responsable de tout ce qui touche aux utilisateurs, channels, et commandes
Gère le parsing et l'implémentation du protocole IRC

# 📊 Vue d'ensemble des dépendances

PHASE 1 (ENSEMBLE) : Setup initial
    ↓
PHASE 2A (A)       PHASE 2B (B)
Socket & Poll  →   Classe Client
    ↓                  ↓
    └──────┬───────────┘
           ↓
    INTÉGRATION 2
           ↓
PHASE 3A (A)       PHASE 3B (B)
Parsing        →   Authentification
    ↓                  ↓
    └──────┬───────────┘
           ↓
    INTÉGRATION 3
           ↓
PHASE 4A (A)       PHASE 4B (B)
Channels       →   JOIN/PART
    ↓                  ↓
    └──────┬───────────┘
           ↓
PHASE 4C (ENSEMBLE) : PRIVMSG
           ↓
PHASE 5A (A)       PHASE 5B (B)
KICK/INVITE    →   TOPIC/MODE
    ↓                  ↓
    └──────┬───────────┘
           ↓
PHASE 6 (ENSEMBLE) : Tests & validation

# 📅 PHASE 1 : Setup initial (3-4 jours)
👥 ENSEMBLE - Obligatoire
Pourquoi ensemble ?
Vous devez tous les deux comprendre la structure de base du projet. C'est la fondation.
Ce qu'on fait

 Setup du repository Git
 Structure des dossiers (includes/, srcs/)
 Makefile de base
 main.cpp (parsing des arguments)
 Créer une socket serveur basique
 La faire écouter sur un port
 Accepter UNE connexion
 Recevoir et afficher un message
 Répondre "Echo"

Validation avant de continuer

✅ nc localhost 6667 se connecte
✅ Un message envoyé s'affiche côté serveur
✅ Le serveur répond

⏱️ Durée : 3-4 jours

# 📅 PHASE 2 : Multi-clients (4-5 jours)
🔀 Travail en parallèle possible
**PERSONNE A : Gestion des sockets multiples**
Ce qu'elle fait

 Adapter poll() pour surveiller N sockets
 Gérer l'ajout d'un nouveau client
 Gérer la suppression d'un client
 Détecter les déconnexions
 Maintenir un vector de clients

Livrables

Server::addClient(int fd)
Server::removeClient(int fd)
Server::run() avec poll() multi-sockets

Dépendances
❌ Aucune (peut commencer après Phase 1)

**PERSONNE B : Classe Client et buffers**
Ce qu'elle fait

 Créer la classe Client complète
 Gérer le buffer de réception
 Implémenter l'extraction de messages complets
 Gérer les messages partiels (split sur \r\n)

Livrables

Client classe avec :

int _fd
std::string _recvBuffer
std::string _nickname
std::string _username
bool _authenticated
bool _registered


Client::appendToBuffer()
Client::extractMessage()

Dépendances
❌ Aucune (peut commencer après Phase 1)


INTÉGRATION 2 (1 jour ensemble)
Ce qu'on fait

 Personne A intègre la classe Client de Personne B
 Modifier Server::handleClient() pour utiliser les buffers
 Tester avec 5 clients nc simultanés
 Tester les messages partiels avec ctrl+D

Validation

✅ 5 clients connectés simultanément
✅ Messages partiels reconstruits correctement
✅ Déconnexion d'un client n'affecte pas les autres
✅ Pas de memory leaks (valgrind)

⏱️ Durée Phase 2 : 4-5 jours

# 📅 PHASE 3 : Authentification (5-6 jours)
🔀 Travail en parallèle possible
**PERSONNE A : Parsing des commandes IRC**
Ce qu'elle fait

 Créer la structure Command
 Implémenter parseMessage(string) → Command
 Parser le prefix optionnel
 Parser la commande
 Parser les paramètres (gérer le trailing parameter avec ':')
 Créer le système de dispatch des commandes

Livrables

struct Command { prefix, command, params[] }
Command parseMessage(const string& msg)
CommandHandler classe de base

Exemples à parser
"PASS secret"              → command="PASS", params=["secret"]
"NICK alice"               → command="NICK", params=["alice"]
"USER alice 0 * :Alice W"  → command="USER", params=["alice","0","*","Alice W"]

Dépendances
❌ Aucune (peut commencer après Intégration 2)

**PERSONNE B : Commandes d'authentification**
Ce qu'elle fait

 Créer le fichier des réponses IRC (codes numériques)
 Implémenter execPass()
 Implémenter execNick()
 Implémenter execUser()
 Gérer les erreurs (password incorrect, nickname pris, etc.)
 Envoyer le message de bienvenue (001)

Livrables

Replies.cpp avec les fonctions helper pour codes IRC
execPass(), execNick(), execUser()
Codes : 001, 461, 462, 464, 431, 432, 433

Dépendances
⚠️ A besoin du parser de Personne A
→ Peut commencer à préparer la structure en attendant

INTÉGRATION 3 (1 jour ensemble)
Ce qu'on fait

 Personne A intègre les commandes de Personne B
 Connecter le parser au dispatcher
 Tester le flow complet d'authentification
 Vérifier tous les cas d'erreur

Validation

✅ Impossible d'envoyer des commandes sans PASS d'abord
✅ Les nicknames dupliqués sont rejetés
✅ Le message de bienvenue (001) est reçu après PASS+NICK+USER
✅ Tous les codes d'erreur fonctionnent

⏱️ Durée Phase 3 : 5-6 jours

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


**PERSONNE B : Commandes JOIN et PART**
Ce qu'elle fait

 Implémenter execJoin()
 Gérer la création de channel si inexistant
 Envoyer tous les messages appropriés (confirmation, topic, liste membres)
 Implémenter execPart()
 Gérer la suppression du channel si vide

Livrables

execJoin() complet avec tous les codes de réponse
execPart() complet
Codes : 331, 332, 353, 366, 442, 403

Dépendances
⚠️ A besoin de la classe Channel de Personne A
→ Peut préparer la logique en attendant

**PHASE 4C : PRIVMSG (2 jours ensemble)**
Pourquoi ensemble ?
PRIVMSG touche à la fois les channels (A) et les clients (B). C'est plus simple de le faire ensemble.
Ce qu'on fait

 Parser la cible (nickname ou #channel)
 Si nickname : trouver le client, envoyer uniquement à lui
 Si channel : broadcaster à tous les membres sauf l'émetteur
 Gérer les erreurs (cible introuvable, pas dans le channel)

Validation

✅ Messages privés entre users fonctionnent
✅ Messages de channel sont broadcast à tous
✅ L'émetteur ne reçoit pas son propre message de channel

⏱️ Durée Phase 4 : 6-7 jours

# 📅 PHASE 5 : Commandes opérateur (6-7 jours)
🔀 Travail en parallèle
**PERSONNE A : KICK et INVITE**
Ce qu'elle fait

 Implémenter execKick()
   -Vérifier que l'émetteur est opérateur
   -Retirer la cible du channel
   -Broadcaster le kick

 Implémenter execInvite()
   -Ajouter à la liste des invités du channel
   -Envoyer les notifications

Livrables
   -execKick() complet
   -execInvite() complet
   -Codes : 441, 482, 341

Dépendances
❌ Aucune (peut commencer après Phase 4)


**PERSONNE B : TOPIC et MODE**
Ce qu'elle fait

 Implémenter execTopic()
   -Afficher le topic
   -Changer le topic (respecter mode +t)


 Implémenter execMode()
   -Parser les modes (+/- et lettres)
   -Implémenter +i/-i (invite-only)
   -Implémenter +t/-t (topic restriction)
   -Implémenter +k/-k (key/password)
   -Implémenter +o/-o (operator)
   -Implémenter +l/-l (user limit)

 Modifier JOIN pour respecter les modes

Livrables
   -execTopic() complet
   -execMode() complet avec les 5 flags
   -Modifications dans JOIN pour vérifier i, k, l

Dépendances
❌ Aucune (peut commencer après Phase 4)


INTÉGRATION 5 (1 jour ensemble)
Ce qu'on fait

 Tester toutes les commandes opérateur
 Tester les combinaisons de modes
 Scénario complexe avec invite-only + password + limit

Validation

✅ Seuls les opérateurs peuvent KICK/INVITE/TOPIC (selon +t)/MODE
✅ Tous les modes fonctionnent individuellement
✅ Les modes se combinent correctement
✅ JOIN respecte tous les modes

⏱️ Durée Phase 5 : 6-7 jours

# 📅 PHASE 6 : Tests et robustesse (5-6 jours)
👥 ENSEMBLE - Obligatoire
Pourquoi ensemble ?
C'est le moment de détecter TOUS les bugs. Deux paires d'yeux valent mieux qu'une.
Ce qu'on fait

 Tests avec irssi/HexChat
 Tests avec plusieurs clients simultanés
 Test des messages partiels (sujet)
 Tests de charge (50+ clients)
 Tests d'erreurs (déconnexions brutales, données invalides)
 Valgrind pour les memory leaks
 Revue de code complète
 Vérification conformité au sujet
 Documentation (README)

Validation finale

✅ Aucun crash
✅ Aucune fuite mémoire
✅ Toutes les commandes fonctionnent avec irssi
✅ Test du sujet (nc avec ctrl+D) réussi
✅ Code propre et commenté

⏱️ Durée Phase 6 : 5-6 jours

*⚖️ Équilibrage de la charge*
Volume de travail par personne
PERSONNE A (Infrastructure)

Phase 2 : Gestion multi-sockets (complexe)
Phase 3 : Parsing IRC (moyen)
Phase 4 : Classe Channel (moyen)
Phase 5 : KICK/INVITE (simple)

PERSONNE B (Logique métier)

Phase 2 : Classe Client + buffers (moyen)
Phase 3 : Auth PASS/NICK/USER (moyen)
Phase 4 : JOIN/PART (complexe)
Phase 5 : TOPIC/MODE (très complexe)

*🔄 Points de synchronisation obligatoires*
Vous DEVEZ vous retrouver à ces moments :

Après Phase 1 : Comprendre la base ensemble
Intégration 2 : Fusionner sockets + buffers
Intégration 3 : Fusionner parser + auth
Phase 4C : PRIVMSG ensemble
Intégration 5 : Tester les modes ensemble
Phase 6 complète : Tests finaux ensemble