#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // ! WIN32_LEAN_AND_MEAN

#include <thread>
#include <map>
#include <signal.h>
#include <cstdlib>
#include <csignal>

#include "ServerListener.h"
#include "HeaderParser.h"
#include "AnswerMaker.h"



const char* message =
	"HTTP/1.1 200 OK\r\n"
	"Server: http_server\r\n"
	"Connection: keep-alive\r\n"
	"Content-type: text/html; charset=utf-8\r\n"
	"Keep-Alive: timeout=5, max=1000\r\n"
	"Content-Length: 154\r\n"
	"\r\n"
	"<HTML><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><TITLE>Successfully connected</TITLE><BODY>Successfully connected</BODY></HTML>"
	"\r\n";

const char* internal_error_message =
"HTTP/1.1 500 Internal Server Error\r\n"
"Server: http_server\r\n"
"Connection: keep-alive\r\n"
"Content-type: text/html; charset=utf-8\r\n"
"Keep-Alive: timeout=5, max=1000\r\n"
"Content-Length: 178\r\n\r\n"
"<html>\r\n"
"<head>\r\n"
"<meta http-equiv=\"content-type\"; charset=\"utf-8\"/>\r\n"
"<title>Internal Server Error</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h1>500: Internal Server Error</h1>\r\n"
"</body>\r\n"
"</html>\r\n";

ServerListener::ServerListener(int port, size_t buffer_size)
{
	this->port = port;
	this->buffer_size = buffer_size;
	this->isServerRunning = false;
	this->ListenSocket = INVALID_SOCKET;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		throw WSAStartupException(result);
		ExitProcess(EXIT_FAILURE);
	}
	
	std::cout << "WSA Started" << std::endl;
}

void ServerListener::run()
{
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {

		throw SocketErrorException("getaddrinfo() failed with error: ", iResult);
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		throw SocketErrorException("Error at socket() with error: ", WSAGetLastError());
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		throw SocketErrorException("Bind failed with error: ", WSAGetLastError());
	}
	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		WSACleanup();
		throw SocketErrorException("Listen failed with error: ", WSAGetLastError());
	}

	std::map<SOCKET, std::thread> threads;

	// Wait until accept a new connection and create new serving thread for it
	isServerRunning = true;
	while (isServerRunning)
	{
		SOCKET ClientSocket;
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			closesocket(ListenSocket);
			WSACleanup();
			throw SocketErrorException("ClientScocket accept error: ", WSAGetLastError());
		}
		threads[ClientSocket] = std::thread(ServerListener::clientHandler, ClientSocket, buffer_size);
	}
	stop();
}

void ServerListener::stop()
{
	std::cout << "Shutting down" << std::endl;
	// Shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(ListenSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		throw SocketErrorException("Shutdown failed with error: ", WSAGetLastError());
	}
	closesocket(ListenSocket);
	WSACleanup();

	ExitProcess(EXIT_SUCCESS);
}


void ServerListener::clientHandler(SOCKET ClientSocket, size_t buffer_size)
{
	char recvbuf[4096] = {0};
	int iResult;

	do {
		iResult = recv(ClientSocket, recvbuf, 4096, 0);

		if (iResult > 0)
		{
			HeaderParser parser(recvbuf);

			std::string target = parser.getTarget();
			std::map<std::string, std::string> headers = parser.getHeaders();

			AnswerMaker ans;
			try
			{
				ans.createAnswer(target, headers);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				std::cout << e.what() << std::endl;
				int iResult = send(ClientSocket, internal_error_message, strlen(internal_error_message), 0);
				if (iResult == SOCKET_ERROR)
				{
					WSACleanup();
					throw SocketErrorException("Send failed with error: ", WSAGetLastError());
				}
				ExitThread(EXIT_FAILURE);
			}
			
			
			std::string answer;

			try
			{
				char* filebuffer = new char[ans.filesize];

				if (ans.contentType == "text/html")
				{
					iResult = send(ClientSocket, ans.answer.c_str(), strlen(ans.answer.c_str()), 0);
					if (iResult == SOCKET_ERROR)
					{
						closesocket(ClientSocket);
						WSACleanup();
						throw SocketErrorException("Send failed with error: ", WSAGetLastError());
					}
				}
				else
				{
					iResult = send(ClientSocket, ans.header.c_str(), strlen(ans.header.c_str()), 0);
					if (iResult == SOCKET_ERROR)
					{
						closesocket(ClientSocket);
						WSACleanup();
						throw SocketErrorException("Send failed with error: ", WSAGetLastError());
					}
					while (!ans.byteFile.eof())
					{
						ans.byteFile.read(filebuffer, ans.filesize);
						iResult = send(ClientSocket, filebuffer, ans.filesize, 0);
						if (iResult == SOCKET_ERROR)
						{
							closesocket(ClientSocket);
							WSACleanup();
							throw SocketErrorException("Send failed with error: ", WSAGetLastError());
						}
					}
				}
				

				printf("Bytes sent: %ld\n", iResult);
			}
			catch (std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

	} while (iResult > 0);
}

ServerListener::~ServerListener()
{
	WSACleanup();
}