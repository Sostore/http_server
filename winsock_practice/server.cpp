#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
// #pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void)
{
	WSADATA wsaData;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		ExitProcess(EXIT_FAILURE);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}
	freeaddrinfo(result);

	std::cout << "Listening socket" << std::endl;
	do {
		iResult = listen(ListenSocket, SOMAXCONN);

		if (iResult == SOCKET_ERROR)
		{
			printf("listen failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			ExitProcess(EXIT_FAILURE);
		}

		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("accept failed: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			ExitProcess(EXIT_FAILURE);
		}

		// Recveive until the peer shuts down the connection
		do {
			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				printf("Bytes received: %d\n", iResult);

				printf("Message: %.*s\n", iResult, recvbuf);

				// Echo the buffer back to the sender
				iSendResult = send(ClientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR)
				{
					printf("send failed: %ld\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					ExitProcess(EXIT_FAILURE);
				}
				printf("Bytes sent: %ld\n", iSendResult);
			}
			else if (iResult == 0)
			{
				printf("Connection closing...\n");
			}
			else
			{
				printf("Receive failed: %ld\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				ExitProcess(EXIT_FAILURE);
			}
		} while (iResult > 0);
		
	} while (iResult != SOCKET_ERROR);
	

	

	// Accept a client socket
	

	std::cout << "Successfully accepted connection" << std::endl;

	

	std::cout << "Shutting down" << std::endl;
	// Shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		ExitProcess(EXIT_FAILURE);
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	ExitProcess(EXIT_SUCCESS);
}