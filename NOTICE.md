
# Guide d'utilisation du serveur IRC

## Démarrage du serveur

Pour démarrer le serveur, utilisez la commande suivante :

```sh
./ircserv 6667 1
```

## Connexion des clients

Vous pouvez connecter des clients au serveur de différentes manières :

### Netcat

```sh
nc localhost 6667
```

### Irssi

Lancez `irssi`, puis tapez :

```sh
/connect localhost 6667 1
```

## Liste des commandes

- **Aide :**

  ```sh
  /help
  ```

- **Envoyer un message privé :**

  ```sh
  /PRIVMSG <nom de l'utilisateur> [message]
  ``

- **Rejoindre un channel :**

  ```sh
  /JOIN [nom du channel]
  ```  

- **Changer de pseudo :**

  ```sh
  /nick [nouveau pseudo]
  ```

- **Lister les channels :**

  ```sh
  /list
  ```

- **Message privé :**

  ```sh
  /msg [nom de l'utilisateur] [message]
  ```

- **Rejoindre un channel :**

  ```sh
  /join [nom du channel]
  ```

- **Quitter le serveur :**

  ```sh
  /quit [optionnel - raison]
  ```

- **Quitter un channel :**

  ```sh
  /part [nom du channel]
  ```

- **Changer le sujet du channel :**

  ```sh
  /topic [nouveau sujet]
  ```

- **Exclure un utilisateur du channel :**

  ```sh
  /kick [nom de l'utilisateur]
  ```

- **Inviter un utilisateur dans un channel :**

  ```sh
  /invite [nom de l'utilisateur]
  ```

## MODE

- **Changer le mode d'un utilisateur ou d'un channel :**

  ```sh
  /mode [nom de l'utilisateur] [nom du channel] [mode]
  ```


### Description des modes

1. **Mode i (Invite Only)**
   - **Description** : Rend le channel accessible uniquement sur invitation.
   - **Usage** : Seuls les utilisateurs invités peuvent rejoindre le channel.
   - **Commande** :
     ```sh
     /mode #nom_du_channel +i
     /mode #nom_du_channel -i
     ```

2. **Mode t (Topic Protection)**
   - **Description** : Seuls les opérateurs de channel peuvent changer le sujet du channel.
   - **Usage** : Empêche les utilisateurs non opérateurs de modifier le sujet du channel.
   - **Commande** :
     ```sh
     /mode #nom_du_channel +t
     /mode #nom_du_channel -t
     ```

3. **Mode k (Channel Key)**
   - **Description** : Protège le channel avec un mot de passe.
   - **Usage** : Les utilisateurs doivent fournir le mot de passe pour rejoindre le channel.
   - **Commande** :
     ```sh
     /mode #nom_du_channel +k [mot_de_passe]
     /mode #nom_du_channel -k
     ```

4. **Mode o (Operator)**
   - **Description** : Donne ou retire le statut d'opérateur à un utilisateur dans le channel.
   - **Usage** : Les opérateurs ont des privilèges supplémentaires comme changer le sujet ou expulser des utilisateurs.
   - **Commande** :
     ```sh
     /mode #nom_du_channel +o [nom_de_l'utilisateur]
     /mode #nom_du_channel -o [nom_de_l'utilisateur]
     ```

5. **Mode l (Channel Limit)**
   - **Description** : Limite le nombre d'utilisateurs pouvant rejoindre le channel.
   - **Usage** : Empêche plus d'utilisateurs que la limite spécifiée de rejoindre le channel.
   - **Commande** :
     ```sh
     /mode #nom_du_channel +l [limite_d'utilisateur]
## Mise en pause et reprise du client

Pour mettre en pause le client, utilisez `Ctrl+Z`. Pour le reprendre, tapez :

```sh
fg
```