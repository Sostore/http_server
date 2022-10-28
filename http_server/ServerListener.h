#ifndef SERVERLISTENER_H
#define SERVERLISTENER_H

#define DEFAULT_PORT "25565"
#define DEFAULT_IP "127.0.0.1"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

#ifndef SERVEREXCEPTIONS_H
#define SERVEREXCEPTIONS_H
#endif

#pragma comment (lib, "Ws2_32.lib")

#pragma once
class ServerListener
{
	int port;
	SOCKET ListenSocket = INVALID_SOCKET;
	size_t buffer_size;
	bool isServerRunning;

	static void clientHandler(SOCKET ClientSocket, size_t buffer_size);

public:
	ServerListener(int port = 80, size_t buffer_size = 512);
	void run();
	void stop();
	~ServerListener();
};

#endif