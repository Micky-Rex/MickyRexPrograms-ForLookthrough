#include<map>
#include<math.h>
#include<iostream>
#include<WinSock2.h>
#pragma comment(dll,"ws2_32.dll")
#define MaxClient 2
#define MaxBufSize 50
#define _CRT_SECURE_NO_WARINGS
using namespace std;
int Connect=0;
double StartGame=0;
const int PORT = 8000;
int playerxlist[]={10,610};
bool OverGame=false;
bool Play_Sound=false;
struct Player{
	int x,y;
};
map<int,Player> player;
struct Ball{
	int x,y,speedx,speedy;
	void init(){
		x=320;
		y=180;
		speedx=2;
		speedy=2;
		return;
	}
	void move(SOCKET* ClientSocket){
		//移动和边界碰撞 
		Play_Sound=false;
		x+=speedx*(2/Connect*1.0);
		y+=speedy*(2/Connect*1.0);
		if(x<20 or x>640-20) speedx=0-speedx,Play_Sound=true;
		if(y<20 or y>360-20) speedy=0-speedy,Play_Sound=true;
		//坐标超界检查和校正 
		if(x<20 or x>640-20) OverGame=true;
		if(y<20) y=20;
		if(y>360-20) y=360-20;
		//玩家碰撞检测 
		if(y<=player[*ClientSocket].y+50 and y>=player[*ClientSocket].y and x<=player[*ClientSocket].x+40 and x>=player[*ClientSocket].x) speedx=0-speedx,Play_Sound=true;
		else if(y<=player[*ClientSocket].y+50 and y>=player[*ClientSocket].y and x<=player[*ClientSocket].x and x>=player[*ClientSocket].x-20) speedx=0-speedx,Play_Sound=true;
		return;
	}
}ball;

//服务线程
DWORD WINAPI ServerThread(LPVOID lpParameter){
	OverGame=false;
	SOCKET *ClientSocket=(SOCKET*)lpParameter;
	if(Connect>MaxClient){
		send(*ClientSocket,"客户端数量超限，拒绝连接",sizeof("客户端数量超限，拒绝连接"),0);
		Connect--;
		closesocket(*ClientSocket);
		free(ClientSocket);
		return 0;
	}else{
		send(*ClientSocket,"连接服务器成功\n",sizeof("连接服务器成功\n"),0);
		if(Connect>2) player.clear();
	}
	StartGame++;
	while(Connect<2);
	player.clear(),ball.init();
//	while(StartGame<2);
	StartGame=0;
	int receByt=0;
	int RecvBuf[MaxBufSize];
	int SendBuf[MaxBufSize];
	while(1){
		//接收消息 
		map<int,Player>::iterator it;
		receByt = recv(*ClientSocket,(char*)RecvBuf,MaxBufSize*sizeof(int),NULL);
		int i=0;
		if(receByt>0){
			//接收到消息 
			for(it=player.begin();it!=player.end();it++,i++){
				if(it->first==*ClientSocket) player[*ClientSocket].x=playerxlist[i];
			}
			player[*ClientSocket].y=RecvBuf[0];
			
		}else{
			//退出连接 
			cout<<"客户端"<<*ClientSocket<<"退出连接！"<<endl;
			player.clear();
			Connect--;
			break;
		}
		memset(RecvBuf,0,sizeof(RecvBuf));
		for(it=player.begin();it!=player.end();it++) if(it->first!=*ClientSocket)break;
		//拼接和发送消息 
		ball.move(ClientSocket);
		SendBuf[0]=ball.x;
		SendBuf[1]=ball.y;
		SendBuf[2]=it->second.x;
		SendBuf[3]=it->second.y;
		SendBuf[4]=player[*ClientSocket].x;
		SendBuf[5]=OverGame ? 1 : 0;
		SendBuf[6]=PlaySound ? 1 : 0;
		int k=send(*ClientSocket,(char*)SendBuf,MaxBufSize*sizeof(int),0);
//		k=send(*ClientSocket,(char*)SendBuf,3,0);
		if(k<0){
			cout<<"发送消息至客户端"<<*ClientSocket<<"失败"<<endl;
		}
		memset(SendBuf,0,sizeof(SendBuf));
		Sleep(5);
	}
	StartGame--;
	closesocket(*ClientSocket);
	free(ClientSocket);
	return 0;
}
int main(){
	WSAData wsd;
	WSAStartup(MAKEWORD(2,2),&wsd);
	SOCKET ListenSocket = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family=AF_INET;
	ListenAddr.sin_addr.S_un.S_addr=INADDR_ANY;//表示填入本机ip
	ListenAddr.sin_port=htons(PORT);
	int n;
	n=bind(ListenSocket,(LPSOCKADDR)&ListenAddr,sizeof(ListenAddr));
	ball.init();
	if(n==SOCKET_ERROR){
		cout<<"端口绑定失败！"<<endl;
		return -1;
	}
	else{
		cout<<"端口绑定成功："<<PORT<<endl;
	}
	int l =listen(ListenSocket,20);
	cout<<"服务端准备就绪，等待连接请求"<<endl;
	while(1){
		//循环接收客户端连接请求并创建服务线程
		SOCKET *ClientSocket = new SOCKET;
		ClientSocket=(SOCKET*)malloc(sizeof(SOCKET));
		
		//接收客户端连接请求
		
		int SockAddrlen = sizeof(sockaddr);
		*ClientSocket = accept(ListenSocket,0,0);
		cout<<"客户端连接到服务器,socket："<<*ClientSocket<<endl;
		CreateThread(NULL,0,&ServerThread,ClientSocket,0,NULL);
		Connect++;
	}
	closesocket(ListenSocket);
	WSACleanup();
	return(0);
}
