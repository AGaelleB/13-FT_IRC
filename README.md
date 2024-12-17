# 13-FT_IRC

![irc](https://github.com/user-attachments/assets/a9fb5001-b4c4-45c6-ad16-7823905643de)

## Summary

The objective of this project is to reproduce the functionality of an IRC server. We will use a real IRC client to connect to our server and test it.

## Introduction

Internet Relay Chat (IRC) is a text-based communication protocol on the Internet. It is primarily used for instant communication through group discussions via chat channels, but it can also be used for direct communication between two people.

## Mandatory Part

### Program Name

- **ircserv**

### Allowed External Functions

- `socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `poll` (or equivalent)

### General Description

- Develop an IRC server in C++ 98.

# IRC Server User Guide

## Starting the Server

To start the server, use the following command:

```sh
./ircserv 6667 1
```

![Screenshot from 2024-12-17 13-15-04](https://github.com/user-attachments/assets/9fe21d94-1c43-4565-83b5-b00884b33ba6)

## Connecting Clients

You can connect clients to the server in different ways:

### Netcat

```sh
nc localhost 6667
```

![Screenshot from 2024-12-17 13-15-12](https://github.com/user-attachments/assets/a2c8105b-c3b2-4f24-998b-b2be85f422f1)

### Irssi

Launch `irssi` and then type:

```sh
/connect localhost 6667 1
```

![Screenshot from 2024-12-17 13-15-28](https://github.com/user-attachments/assets/0dc48e25-1874-41a8-a711-cd984fc0f493)

## List of Commands

- **Help:**

  ```sh
  /help
  ```

- **Send a private message:**

  ```sh
  /PRIVMSG <username> [message]
  ```

- **Join a channel:**

  ```sh
  /JOIN [channel name]
  ```  

- **Change nickname:**

  ```sh
  /nick [new nickname]
  ```

- **List channels:**

  ```sh
  /list
  ```

- **Private message:**

  ```sh
  /msg [username] [message]
  ```

- **Join a channel:**

  ```sh
  /join [channel name]
  ```

- **Quit the server:**

  ```sh
  /quit [optional - reason]
  ```

- **Leave a channel:**

  ```sh
  /part [channel name]
  ```

- **Change channel topic:**

  ```sh
  /topic [new topic]
  ```

- **Kick a user from a channel:**

  ```sh
  /kick [username]
  ```

- **Invite a user to a channel:**

  ```sh
  /invite [username]
  ```

## MODE

- **Change user or channel mode:**

  ```sh
  /mode [username] [channel name] [mode]
  ```

### Mode Descriptions

1. **Mode i (Invite Only)**
   - **Description**: Makes the channel accessible only by invitation.
   - **Usage**: Only invited users can join the channel.
   - **Command**:
     ```sh
     /mode #channel_name +i
     /mode #channel_name -i
     ```

2. **Mode t (Topic Protection)**
   - **Description**: Only channel operators can change the channel topic.
   - **Usage**: Prevents non-operators from modifying the channel topic.
   - **Command**:
     ```sh
     /mode #channel_name +t
     /mode #channel_name -t
     ```

3. **Mode k (Channel Key)**
   - **Description**: Protects the channel with a password.
   - **Usage**: Users must provide the password to join the channel.
   - **Command**:
     ```sh
     /mode #channel_name +k [password]
     /mode #channel_name -k
     ```

4. **Mode o (Operator)**
   - **Description**: Grants or removes operator status for a user in the channel.
   - **Usage**: Operators have additional privileges such as changing the topic or kicking users.
   - **Command**:
     ```sh
     /mode #channel_name +o [username]
     /mode #channel_name -o [username]
     ```

5. **Mode l (Channel Limit)**
   - **Description**: Limits the number of users allowed in the channel.
   - **Usage**: Prevents more users than the specified limit from joining the channel.
   - **Command**:
     ```sh
     /mode #channel_name +l [user_limit]
     ```

## Pausing and Resuming the Client

To pause the client, use `Ctrl+Z`. To resume, type:

```sh
fg
```

# BONUS

## File Transfer

To send a file between two clients using Irssi and Netcat, we use the DCC (Direct Client-to-Client) protocol, which is specifically designed for this type of transfer over IRC.

### Using Irssi and DCC

- **Command on the sender's computer (Irssi client):**

```sh
/dcc send <recipient_username> <file_path>
```

- **Command on the receiver's computer (Irssi client):**

```sh
/dcc get <sender_username> <file_name>
```

## The Bot

Our IRC bot monitors messages sent in channels and kicks users who use forbidden words. It maintains a list of banned words and checks each message. If a forbidden word is detected, the user is kicked from the channel, and a notification is sent to other members.

1. **Mode b (ban word)**
   - **Description**: The +b mode allows channel operators to add/remove words to the banned words list.
   - **Usage**: Only operators can add/remove forbidden words to better moderate the channel.
   - **Command**:
     ```sh
     /mode #channel_name +b [banned_word]
     /mode #channel_name -b [unbanned_word]
     ```

# Commands for Memory Leaks

Be careful, VS Code leaves file descriptors open, so test in a terminal.

```sh
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./ircserv 6667 1
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes nc localhost 6667
```
