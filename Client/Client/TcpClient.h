#pragma once
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class TcpClient {
	private:
		std::string ipAddress;
		int PORT;
		SOCKET clientSocket = INVALID_SOCKET;

		int initWS();
		SOCKET createSocket();
		int connectServer();
		void communicate();
		void errMsg(std::string msg);
		void clearSocket();

	public:
		TcpClient(std::string addr, int port): ipAddress(addr), PORT(port) {}
		void run();
};

