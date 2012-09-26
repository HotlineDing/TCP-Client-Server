TCP-Client-Server
=================
`Due Date: September 25th by 11:59 pm`

Client/Server socket program to send strings.

Description
-----------

Server runs on a TCP port of your choosing to accept connections from mutiple clients using the fork function, and perform various actions.

### Server

Usage on how to start the server.

`./server 8080`

You can replace the above port with any above 1024 as you wish.

### Client

Usage on how to start the client.

`./client localhost 8080`

You can replace the hostname and port as you wish.

The following is a list of accepted commands:

`GET`
`STORE`
`TRANSLATE`
`EXIT`

Entering any other command will return a '400 Command not valid.'
