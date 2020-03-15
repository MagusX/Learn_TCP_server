#pragma once
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class TcpListener {
	private:
		int PORT;
		SOCKET listeningSocket = INVALID_SOCKET;
		sockaddr_in hint;
		SOCKET clientSocket;

		int initWS();
		SOCKET createSocket();
		int bindSocket();
		int listenConnection();
		void acceptConnection();
		void echoMessage();
		void clearListenSocket();
		void clearClientSocket();
		void errMsg(std::string msg);

	public:
		TcpListener(int port): PORT(port) {}
		void run();
};

