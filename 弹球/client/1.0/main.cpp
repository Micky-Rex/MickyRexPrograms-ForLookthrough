#include <conio.h>
#include <fstream>
#include <iostream>
#include <graphics.h>
#include <WinSock2.h>
#include <mmsystem.h>
#pragma comment(dll,"winmm.dll")
#pragma comment(dll,"ws2_32.dll")
#define KEY_DOWN(VK_NINAME)((GetAsyncKeyState(VK_NINAME)&0x8000)?1:0)  //用来检测按键的点击事件 
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)
using namespace std;
HWND H=GetForegroundWindow();  //获取最顶层窗口的句柄 
POINT P;
int GameOver=0;
int x,y;
char ip[20];

struct Ball{
	int x,y;
}ball;
struct Player{
	int x,y;
}player;


int main() {
	WSADATA wsd;//定义 WSADATA对象
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {//初始化WSA
		WSACleanup();
		return -1;
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		cout << "error:" << WSAGetLastError() << endl;
		WSACleanup();
		return -2;
	}
	ifstream fin("./socket.conf");
	fin.getline(ip,20,';');
	fin.close();
	cout<<"连接至服务器"<<ip<<"......\n";
	SOCKADDR_IN client;
	client.sin_family = AF_INET;
	client.sin_port = htons(8000);
	client.sin_addr.S_un.S_addr = inet_addr(ip);
	int const SERVER_MSG_SIZE = 50;
	int RecvBuf[SERVER_MSG_SIZE];//用户输入的消息
	int SendBuf[SERVER_MSG_SIZE];//要发送给服务器的消息
	//连接服务器失败
	if (connect(clientSocket, (struct sockaddr*) &client, sizeof(client)) < 0) {
		cout << "error:" << WSAGetLastError() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return -3;
	}
	//连接服务器成功
	else {
//		cout << "连接服务器成功......\n" << endl;
		char tmp[SERVER_MSG_SIZE];
		recv(clientSocket,tmp,SERVER_MSG_SIZE,0);
		cout<<"服务器回应："<<tmp<<endl;
		if(strcmp(tmp,"客户端数量超限，拒绝连接")==0){
			cout<<"将自动关闭程序\n";
			closesocket(clientSocket);
			WSACleanup();
			system("pause");
			return 0;
		}
//		system("pause");
//		initgraph(640,360,EW_SHOWCONSOLE);
		initgraph(640,360,EW_SHOWCONSOLE);
//		system("pause");
		memset(RecvBuf, 0, SERVER_MSG_SIZE);
		while (!GameOver) {
			BeginBatchDraw();
			cleardevice();
			memset(SendBuf, 0, SERVER_MSG_SIZE);
			H=GetForegroundWindow();
			GetCursorPos(&P);
			ScreenToClient(H,&P);
//			SendBuf[0]=x;
			SendBuf[0]=y;
			send(clientSocket, (char*)SendBuf, SERVER_MSG_SIZE*sizeof(int), 0);
//			while(RecvBuf[0]==0) ;
			int size = recv(clientSocket, (char*)RecvBuf, SERVER_MSG_SIZE*sizeof(int), 0);
			// this client
			x=RecvBuf[4];
			y=P.y;
			setcolor(WHITE);
			line(x,y,x,y+50);
			line(x,y,x+20,y);
			line(x,y+50,x+20,y+50);
			line(x+20,y,x+20,y+50);
			ball.x=RecvBuf[0];
			ball.y=RecvBuf[1];
			player.x=RecvBuf[2];
			player.y=RecvBuf[3];
			GameOver=RecvBuf[5];
			// other client
			setcolor(YELLOW);
			line(player.x,player.y,player.x,player.y+50);
			line(player.x,player.y,player.x+20,player.y);
			line(player.x,player.y+50,player.x+20,player.y+50);
			line(player.x+20,player.y,player.x+20,player.y+50);
			setcolor(WHITE);
			circle(ball.x,ball.y,20);
			memset(RecvBuf, 0, SERVER_MSG_SIZE);
			FlushBatchDraw();
		}
	}
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
