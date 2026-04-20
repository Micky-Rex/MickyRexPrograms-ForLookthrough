
#include "snake.h"
#include <queue>

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

using namespace std;

queue<pair<SOCKET, string> > waitForPrint;	// 相当于 pair<线程编号, 消息类型>	防止消息间冲突 
SOCKET ListenSocket;
SOCKADDR_IN ListenAddr;

DWORD WINAPI ServerThread(LPVOID lpParameter)
{
	SOCKET *clientSocket = (SOCKET*)lpParameter;
	int receByt = 0;
	char recvBuf[MAX_BUF_SIZE];
	char sendBuf[MAX_BUF_SIZE];
	while(true){
		receByt = recv(*clientSocket,recvBuf,sizeof(recvBuf),0);
		
		if(receByt > 0){
			
		}else{
			waitForPrint.push(make_pair(*clientSocket, "client_exit"));
			while(waitForPrint.front().first != *clientSocket or waitForPrint.front().second != "client_exit");
			printf("Client %d exit!\n",*clientSocket);
			waitForPrint.pop();
			break;
		}
		memset(recvBuf,0,sizeof(recvBuf));
		
		int k = send(*clientSocket,sendBuf,sizeof(sendBuf),0);
		if(k < 0){
			waitForPrint.push(make_pair(*clientSocket, "send_msg_failed"));
			while(waitForPrint.front().first != *clientSocket or waitForPrint.front().second != "send_msg_failed");
			printf("[Error] : Send to client %d failed!\n",*clientSocket);
			waitForPrint.pop();
		}
		memset(sendBuf,0,sizeof(sendBuf));
	}
	closesocket(*clientSocket);
	free(clientSocket);
	return 0;
}

int runGameServer()
{
	closegraph();
	WSAStartup(MAKEWORD(2,2),&wsd);
	ListenSocket = socket(AF_INET,SOCK_STREAM,0);
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	ListenAddr.sin_port = htons(PORT);
	int n = bind(ListenSocket,(LPSOCKADDR)&ListenAddr,sizeof(ListenAddr));
	if(n == SOCKET_ERROR){
		printf("[Error] : Port bind failed!\n");
		return -1;
	}

	int l = listen(ListenSocket,20);
	printf("Server is ready! Waiting for connection...\n");
	while(true){
		SOCKET *clientSocket = new SOCKET;
		clientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		
		int SockAddrlen = sizeof(sockaddr);
		*clientSocket = accept(ListenSocket,0,0);
		printf("A client connected! SOCKET : %d\n", *clientSocket);
		
		CreateThread(NULL,0,&ServerThread,clientSocket,0,NULL);
	}//while
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}
#endif
