#include "TcpClient.h"
#include <iostream>
#define BUFFER_SIZE 4096

int TcpClient::initWS() {
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	return WSAStartup(version, &wsaData);
}

SOCKET TcpClient::createSocket() {
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	return clientSocket;
}

int TcpClient::connectServer() {
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	inet_pton(hint.sin_family, ipAddress.c_str(), &hint.sin_addr);

	return connect(clientSocket, (sockaddr*) &hint, sizeof(hint));
}

void TcpClient::communicate() {
	char buffer[BUFFER_SIZE];
	std::string userInp;

	do {
		// Prompt the user for some text
		std::cout << "> ";
		std::getline(std::cin, userInp);

		if (userInp.size() > 0) {
			// Send the text
			int sendResult = send(clientSocket, userInp.c_str(), userInp.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				// Wait for response
				ZeroMemory(buffer, BUFFER_SIZE);
				int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
				if (bytesReceived >= 0) {
					// Echo response to console
					std::cout << "SERVER> " << std::string(buffer, 0, bytesReceived) << std::endl;
				}
			}
		}

	} while (userInp.size() > 0 && userInp != "/quit");
}

void TcpClient::clearSocket() {
	closesocket(clientSocket);
	WSACleanup();
}

void TcpClient::errMsg(std::string msg) {
	std::cout << msg << ": " << WSAGetLastError() << std::endl;
}

void TcpClient::run() {
	if (initWS() != 0) {
		errMsg("WSAStartup failed");
		return;
	}
	if (createSocket() == INVALID_SOCKET) {
		errMsg("Error at socket()");
		WSACleanup();
		return;
	}
	if (connectServer() == SOCKET_ERROR) {
		errMsg("Unable to connect to server");
		clearSocket();
		return;
	}
	communicate();

	clearSocket();
}