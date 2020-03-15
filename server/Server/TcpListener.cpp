#include "TcpListener.h"
#include <iostream>
#define BUFFER_SIZE 4096

int TcpListener::initWS() {
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2); // Makes a request for version 2.2 of Winsock on the system

	return WSAStartup(version, &wsaData); // Initiates use of WS2_32.dll
}

SOCKET TcpListener::createSocket() {
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	return listeningSocket;
}

int TcpListener::bindSocket() {
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT); // Numbers are stored in memory in network byte order, which is with the most significant byte first.
								 //It will therefore swap the bytes making up the number so that in memory the bytes will be stored in the order.
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	return bind(listeningSocket, (sockaddr*) &hint, sizeof(hint));
}

int TcpListener::listenConnection() {
	return listen(listeningSocket, SOMAXCONN);
}

void TcpListener::acceptConnection() {
	sockaddr_in client;
	int clientSize = sizeof(client);
	// Wait for client to connect
	clientSocket = accept(listeningSocket, (sockaddr*) &client, &clientSize);

	char clientHost[NI_MAXHOST]; // Client's remote name
	char clientPort[NI_MAXSERV]; // Client's port

	ZeroMemory(clientHost, NI_MAXHOST);
	ZeroMemory(clientPort, NI_MAXSERV);

	if (getnameinfo((sockaddr*) &client, clientSize, clientHost, NI_MAXHOST, clientPort, NI_MAXSERV, 0) == 0) {
		std::cout << clientHost << " connected on PORT " << clientPort << std::endl;
	} else {
		inet_ntop(AF_INET, &client.sin_addr, clientHost, NI_MAXHOST);
		std::cout << clientHost << " connected on PORT " << ntohs(client.sin_port) << std::endl;
	}

	closesocket(listeningSocket);
}

void TcpListener::echoMessage() {
	char buffer[BUFFER_SIZE];
	while (true) {
		ZeroMemory(buffer, BUFFER_SIZE);
		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (std::string(buffer, 0, bytesReceived) == "/quit" || bytesReceived == 0) {
			std::cout << "Client disconneted" << std::endl;
			break;
		} else if (bytesReceived == SOCKET_ERROR) {
			errMsg("Error in recv()");
			break;
		}
		std::cout << "Client: " << std::string(buffer, 0, bytesReceived) << std::endl;
		send(clientSocket, buffer, bytesReceived + 1, 0); // Echo message back to client
	}
}

void TcpListener::clearListenSocket() {
	closesocket(listeningSocket);
	WSACleanup();
}

void TcpListener::clearClientSocket() {
	closesocket(clientSocket);
	WSACleanup();
}

void TcpListener::errMsg(std::string msg) {
	std::cout << msg << ": " << WSAGetLastError() << std::endl;
}

void TcpListener::run() {
	if (initWS() != 0) {
		errMsg("WSAStartup failed");
		return;
	}
	if (createSocket() == INVALID_SOCKET) {
		errMsg("Error at socket()");
		WSACleanup();
		return;
	}
	if (bindSocket() == SOCKET_ERROR) {
		errMsg("Bind failed");
		clearListenSocket();
		return;
	} else std::cout << "Server started on PORT " << PORT << std::endl;
	if (listenConnection() == SOCKET_ERROR) {
		errMsg("Listen failed");
		clearListenSocket();
		return;
	}
	acceptConnection();
	echoMessage();
	clearClientSocket();
}