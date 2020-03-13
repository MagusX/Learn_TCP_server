// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "3000" // Server port
#define BUFLEN 1024

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

	struct addrinfo* res = NULL, * ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4 addr family OR AF_INET6 for IPv6
	hints.ai_socktype = SOCK_STREAM;	// Stream socket
	hints.ai_protocol = IPPROTO_TCP;	// TCP protocol
	hints.ai_flags = AI_PASSIVE;		// When set and nodename parameter to the getaddrinfo function is a NULL pointer
	                                    // the IP address portion of the socket address structure is set to INADDR_ANY for IPv4 addresses
										// IN6ADDR_ANY_INIT for IPv6 addresses.

	iRes = getaddrinfo(NULL, PORT, &hints, &res); // Resolve the local address and port to be used by the server
	if (iRes != 0) {
		std::cout << "getaddrinfo failed: " << iRes << std::endl;
		WSACleanup();
		return 1;
	}

	// CREATE SOCKET
	SOCKET ListenSocket = INVALID_SOCKET; // Socket listen for client connections
	// If a server wants to listen on both IPv6and IPv4, two listen sockets must be created
	// One for IPv6and one for IPv4. 
	// These two sockets must be handled separately by the application.

	ListenSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(res);
		WSACleanup();
		return 1;
	}

	// BINDING SOCKET TO IP ADDRESS AND PORT
	iRes = bind(ListenSocket, res->ai_addr, (int) res->ai_addrlen);
	if (iRes == SOCKET_ERROR) {
		std::cout << "Bind failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(res);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// LISTEN TO INCOMING REQUEST
	iRes = listen(ListenSocket, SOMAXCONN);
	if (iRes == SOCKET_ERROR) {
		std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// ACCEPT 1 CONNECTION
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// RECEIVE AND SEND DATA
	char recvBuf[BUFLEN];
	int iSendRes;
	int recvBuflen = BUFLEN;

	// Receive until client connection shut down
	while (true) {
		iRes = recv(ClientSocket, recvBuf, recvBuflen, 0);
		if (iRes > 0) {
			std::cout << "Bytes received: " << iRes << std::endl;
			// Echo the buffer back to the sender
			iSendRes = send(ClientSocket, recvBuf, iRes, 0);
			if (iSendRes == SOCKET_ERROR) {
				std::cout << "Send failed: " << WSAGetLastError() << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Bytes sent: " << iSendRes << std::endl;
		} else if (iRes == 0)
			std::cout << "Connection closing..." << std::endl;
		else {
			std::cout << "Recv failed: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		if (iRes <= 0) break;
	}

	// DISCONNECT SERVER
	iRes = shutdown(ClientSocket, SD_SEND);
	if (iRes == SOCKET_ERROR) {
		std::cout << "Shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// When the client is done receiving data, the closesocket function is called to close the socket.
	// When the client is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}