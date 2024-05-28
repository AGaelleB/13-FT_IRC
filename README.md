# 13-FT_IRC

## Résumé

L’objectif de ce projet est de reproduire le fonctionnement d’un serveur IRC. On utilisera un vrai client IRC afin de se connecter à notre serveur et ainsi de le tester.

## Introduction

Internet Relay Chat (IRC) est un protocole de communication textuel sur Internet. Il sert à la communication instantanée principalement sous la forme de discussions en groupe par l’intermédiaire de canaux de discussion, mais peut aussi être utilisé pour de la communication directe entre deux personnes.

## Partie Obligatoire

### Nom du programme

- **ircserv**

### Fichiers de rendu

- Makefile
- Fichiers de code source : `*.{h, hpp, cpp, tpp, ipp}`
- Un fichier de configuration optionnel

### Makefile

- Cibles requises : `NAME`, `all`, `clean`, `fclean`, `re`

### Arguments du programme

- **port** : Le port d’écoute du serveur pour les connexions entrantes
- **password** : Le mot de passe que les clients doivent fournir pour se connecter au serveur

### Fonctions externes autorisées

- `socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `fcntl`, `poll` (ou équivalent)

### Description générale

- Développer un serveur IRC en C++ 98.
- Pas besoin de développer un client IRC.
- La communication entre serveurs IRC n’est pas à implémenter.
- Le binaire doit être lancé comme suit : `./ircserv <port> <password>`

## Prérequis techniques

- **Gestion de multiples clients simultanément** : Le serveur doit gérer plusieurs clients sans jamais bloquer.
- **Non-bloquant** : Toutes les opérations d’entrée/sortie doivent être non-bloquantes.
- **Une seule fonction de multiplexage** : Utiliser un seul `poll()` (ou équivalent comme `select()`, `kqueue()`, `epoll()`) pour gérer toutes les opérations d’entrée/sortie.
- **Client IRC de référence** : Choisir un client IRC comme référence pour tester notre serveur.

## Fonctionnalités obligatoires

- **Authentification** : Les clients doivent s’authentifier avec le mot de passe.
- **Définir un nickname et un username**
- **Rejoindre un channel**
- **Envoyer et recevoir des messages privés**
- **Transmettre les messages d’un client à tous les membres du channel**
- **Opérateurs et utilisateurs basiques** :
  - `KICK` : Éjecter un client du channel
  - `INVITE` : Inviter un client au channel
  - `TOPIC` : Modifier ou afficher le sujet du channel
  - `MODE` : Changer le mode du channel :
    - `i` : Canal sur invitation uniquement
    - `t` : Restriction de la commande TOPIC aux opérateurs
    - `k` : Clé du canal (mot de passe)
    - `o` : Privilège d’opérateur de canal
    - `l` : Limite d’utilisateurs pour le canal

## Particularités pour MacOS

- Utilisation spécifique de `fcntl()` pour gérer les descripteurs de fichier en mode non-bloquant :
  ```cpp
  fcntl(fd, F_SETFL, O_NONBLOCK);




## Grandes Étapes pour la Réalisation du Projet

### Étape 1 : Mise en Place des Sockets et de la Connexion de Base

1. **Initialisation des sockets**
   - Implémenter l'ouverture et la configuration des sockets (utilisation de `socket()`, `bind()`, `listen()`).
2. **Gérer les connexions entrantes**
   - Implément erla fonction pour accepter les connexions entrantes (`accept()`).
   - S'assurer que les descripteurs de fichiers sont configurés en mode non-bloquant (`fcntl(fd, F_SETFL, O_NONBLOCK)`).
3. **Gérer les déconnexions et les erreurs**
   - Gérer proprement les déconnexions des clients et les erreurs de connexion.

### Étape 2 : Gestion des Clients et de la Communication

1. **Multiplexage d'E/S**
   - Implémenter `poll()` ou son équivalent (`select()`, `kqueue()`, `epoll()`) pour surveiller les descripteurs de fichiers et gérer les événements de lecture/écriture.
2. **Authentification des clients**
   - Implémenter la procédure d'authentification basée sur le mot de passe fourni à la connexion.
3. **Gestion des messages**
   - Implémenter la réception et l'envoi de messages entre le serveur et les clients (`recv()`, `send()`).

### Étape 3 : Implémentation des Fonctionnalités IRC de Base

1. **Gestion des commandes IRC**
   - Implémenter les commandes de base : `NICK`, `USER`, `JOIN`, `PRIVMSG`.
2. **Gestion des channels**
   - Implémenter les fonctionnalités de création, de jonction et de gestion des channels.
3. **Transmission des messages**
   - S'ssurer que les messages envoyés dans un channel sont transmis à tous les membres du channel.

### Étape 4 : Implémentation des Commandes Spécifiques aux Opérateurs

1. **Commandes KICK, INVITE, TOPIC, MODE**
   - Implémenter les commandes spécifiques aux opérateurs de canaux avec les différentes options (`i`, `t`, `k`, `o`, `l`).

### Étape 5 : Tests et Validation

1. **Tests unitaires et fonctionnels**
   - Écrire des tests unitaires pour les différentes fonctionnalités.
   - Utiliser un client IRC de référence pour tester l'intégralité des fonctionnalités de notre serveur.
2. **Test de robustesse**
   - Effectuer des tests de charge pour vous assurer que notre serveur peut gérer plusieurs clients simultanément.
   - Tester la gestion des erreurs et des cas limites (données partielles, bande passante faible, etc.).

### Étape 6 : Bonus (Facultatif)

1. **Implémentation des fonctionnalités bonus**
   - Ajouter l’envoi de fichiers entre les clients.
   - Implémenter un bot pour ajouter des fonctionnalités supplémentaires.

## Planification et Répartition du Temps

- **Semaine 1** : Préparation, configuration de l'environnement, mise en place des sockets et de la connexion de base.
- **Semaine 2** : Gestion des clients, authentification, et communication.
- **Semaine 3** : Implémentation des fonctionnalités IRC de base et gestion des channels.
- **Semaine 4** : Implémentation des commandes des opérateurs et tests unitaires.
- **Semaine 5** : Tests de charge, robustesse, documentation, et nettoyage du code.
- **Semaine 6 (facultatif)** : Implémentation des fonctionnalités bonus.

