// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 3000 // Server port
#define BUFLEN 4096

int main() {
	int iRes;
	
	// INITIALIZE WINSOCK
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2); // Makes a request for version 2.2 of Winsock on the system

	iRes = WSAStartup(version, &wsaData); // Initiates use of WS2_32.dll
	if (iRes != 0) {
		std::cout << "WSAStartup failed: " << iRes << std::endl;
		return 1;
	}

	// CREATE SOCKET
	SOCKET listening = INVALID_SOCKET; // Socket listen for client connections
	// If a server wants to listen on both IPv6and IPv4, two listen sockets must be created
	// One for IPv6and one for IPv4. 
	// These two sockets must be handled separately by the application.

	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);	// Numbers are stored in memory in network byte order, which is with the most significant byte first.
									// It will therefore swap the bytes making up the number so that in memory the bytes will be stored in the order.
	hint.sin_addr.S_un.S_addr = INADDR_ANY;


	// BINDING SOCKET TO IP ADDRESS AND PORT
	iRes = bind(listening, (sockaddr*) &hint, sizeof(hint));
	if (iRes == SOCKET_ERROR) {
		std::cout << "Bind failed: " << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	} else
		std::cout << "Server started on PORT " << hint.sin_port << std::endl;

	// LISTEN TO INCOMING REQUEST
	iRes = listen(listening, SOMAXCONN);
	if (iRes == SOCKET_ERROR) {
		std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	}

	// WAIT FOR A CONNECTION
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(listening, (sockaddr*) &client, &clientSize);

	char clientHost[NI_MAXHOST]; // Client's remote name
	char clientPort[NI_MAXSERV]; // Client's port

	ZeroMemory(clientHost, NI_MAXHOST);
	ZeroMemory(clientPort, NI_MAXSERV);

	if (getnameinfo((sockaddr*) &client, clientSize, clientHost, NI_MAXHOST, clientPort, NI_MAXSERV, 0) == 0)
		std::cout << clientHost << " connected on PORT " << clientPort << std::endl;
	else {
		inet_ntop(AF_INET, &client.sin_addr, clientHost, NI_MAXHOST);
		std::cout << clientHost << " connected on PORT " << ntohs(client.sin_port) << std::endl;
	}

	closesocket(listening);

	// ACCEPT CONNECTION AND ECHO MESSAGE
	char buffer[BUFLEN];
	while (true) {
		ZeroMemory(buffer, BUFLEN);
		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buffer, BUFLEN, 0);
		if (bytesReceived == 0) {
			std::cout << "Client disconneted" << std::endl;
			break;
		} else if (bytesReceived == SOCKET_ERROR) {
			std::cout << "Error in recv() " << WSAGetLastError() << std::endl;
			break;
		}
		std::cout << "Client: " << std::string(buffer, 0, bytesReceived) << std::endl;
		send(clientSocket, buffer, bytesReceived + 1, 0); // Echo message back to client
	}

	// When the client is done receiving data, the closesocket function is called to close the socket.
	// When the client is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
