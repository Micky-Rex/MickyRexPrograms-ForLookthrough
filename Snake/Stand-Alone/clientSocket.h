
#include "snake.h"

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

using namespace std;

SOCKET clientSocket;
SOCKADDR_IN client;

int connectServer(string sIP)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		WSACleanup();
		return -1;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("[Error] : %d\n", WSAGetLastError());
		WSACleanup();
		return -2;
	}
	printf("Connect to Server......\n");
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.S_un.S_addr = inet_addr(sIP.c_str());
	//连接服务器失败
	if (connect(clientSocket, (struct sockaddr*) &client, sizeof(client)) < 0)
	{
		int code;
		printf("[Error] : %d\n", code = WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		if(code == 10022 or code == 10044 or code == 10051)
		{
			printf("[Error] : Invalid IP!\n");
		}
		return -3;
	}
	printf("Success!\n");
	return 0;
}

int __sync()
{
	
	return 0;
}
#endif
