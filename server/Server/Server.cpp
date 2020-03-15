// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "TcpListener.h"

int main() {
	TcpListener server(3000);
	server.run();
	return 0;
}
