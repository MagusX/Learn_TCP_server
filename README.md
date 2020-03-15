# Learn_TCP_server
TCP server using winsock2


In TCP:
* (listeningSocket) A socket which listens for incoming connections from clients. It's not connected with anyone. Its only job is to listen for incoming connections if any.
* (clientSocket) When the listening socket gets a request, it stores the address & port of the client in a backlog and asks the server to check if the connection from this client should be accepted. If the connection is accepted, a NEW socket is created bound to the same port as it has, packets are sent to the server after the 3-way handshake of TCP is completed, they get routed to this NEW socket.