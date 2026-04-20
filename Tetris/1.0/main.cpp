/*
	Name: Tetris
	Author: MickyRex
	Date: 03/08/23 21:26
*/
#include<ctime>
#include<queue>
#include<math.h>
#include<vector>
#include<stdio.h>
#include<iostream>
#include<graphics.h>
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)
using namespace std;

const int N=30;
int Score=0,timer=0;
bool GameOver=false,RunCode=false;
bool g[N][N];

void PaintMap();
void PaintGrid(int x,int y,int a,bool show){
	setcolor(show?WHITE:RGB(123,123,123));
	setfillcolor(show?RGB(0,119,230):BLACK);
	fillrectangle(x,y,x+a,y+a);
	return;
}


struct Grid{
	int type;
	int x,y,dir;
	int shape[28][25]=	// 方块形状定义 
	{
		//I
		{
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,0,0,
			0,1,1,1,1,
			0,0,0,0,0,
			0,0,0,0,0,
		},
		{
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,0,0,
			0,1,1,1,1,
			0,0,0,0,0,
			0,0,0,0,0
		},
		//L
		{
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,1,1,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,0,0,
			0,1,1,1,0,
			0,1,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,1,0,
			0,1,1,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		//O
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		//T
		{
			0,0,0,0,0,
			0,0,1,0,0,
			0,1,1,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,1,1,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,0,0,
			0,1,1,1,0,
			0,0,1,0,0,
			0,0,0,0,0,
		},
		{
			0,0,0,0,0,
			0,0,1,0,0,
			0,1,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		//Z
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,0,1,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,1,0,
			0,0,1,1,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,0,0,
			0,0,1,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,0,1,0,
			0,0,1,1,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		// 反 L 
		{
			0,0,0,0,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,1,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,0,0,0,
			0,1,1,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,1,1,0,
			0,0,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,1,1,0,
			0,0,0,1,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		// 反 Z 
		{
			0,0,0,0,0,
			0,0,1,1,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,0,0,0,
			0,1,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,0,1,1,0,
			0,1,1,0,0,
			0,0,0,0,0,
			0,0,0,0,0
		},
		{
			0,0,0,0,0,
			0,1,0,0,0,
			0,1,1,0,0,
			0,0,1,0,0,
			0,0,0,0,0
		}
	};
	int dis[7][4][4]=	// 四个方向方格到 5*5 图像边框的距离 
	{
		{
			{0,2,1,2},
			{2,0,2,1},
			{0,2,1,2},
			{2,0,2,1}
		},
		{
			{1,1,1,2},
			{2,1,1,1},
			{1,2,1,1},
			{1,1,2,1}
		},
		{
			{1,2,2,1},
			{1,2,2,1},
			{1,2,2,1},
			{1,2,2,1}
		},
		{
			{1,1,2,1},
			{1,1,1,2},
			{2,1,1,1},
			{1,2,1,1}
		},
		{
			{1,1,2,1},
			{1,1,1,2},
			{1,1,2,1},
			{1,1,1,2}
		},
		{
			{1,2,1,1},
			{1,1,2,1},
			{1,1,1,2},
			{1,1,2,1}
		},
		{
			{1,1,2,1},
			{1,2,1,1},
			{1,1,2,1},
			{1,2,1,1}
		}
	};
	int hit[7][4][5]=	//每列最下方方格据上边界的距离 (单方向碰撞箱)
	{
		{
			{0,0,4,0,0},
			{0,3,3,3,3},
			{0,0,4,0,0},
			{0,3,3,3,3}
		},
		{
			{0,0,4,4,0},
			{0,4,3,3,0},
			{0,2,4,0,0},
			{0,3,3,3,0}
		},
		{
			{0,3,3,0,0},
			{0,3,3,0,0},
			{0,3,3,0,0},
			{0,3,3,0,0}
		},
		{
			{0,3,3,3,0},
			{0,0,4,3,0},
			{0,3,4,3,0},
			{0,3,4,0,0}
		},
		{
			{0,2,3,3,0},
			{0,0,4,3,0},
			{0,2,3,3,0},
			{0,0,4,3,0}
		},
		{
			{0,4,4,0,0},
			{0,3,3,3,0},
			{0,0,4,2,0},
			{0,2,2,3,0}
		},
		{
			{0,3,3,2,0},
			{0,3,4,0,0},
			{0,3,3,2,0},
			{0,3,4,0,0}
		}
	};
	//left and right hit box
	int lrhit[7][4][2][5]=
	{
		{
			{{3,3,3,3,0},{3,3,3,3,0}},
			{{0,0,4,0,0},{0,0,5,0,0}},
			{{3,3,3,3,0},{3,3,3,3,0}},
			{{0,0,4,0,0},{0,0,5,0,0}}
		},
		{
			{{0,3,3,3,0},{0,3,3,4,0}},
			{{0,0,4,4,0},{0,0,4,2,0}},
			{{0,4,3,3,0},{0,3,3,3,0}},
			{{0,2,4,0,0},{0,4,4,0,0}}
		},
		{
			{{0,4,4,0,0},{0,3,3,0,0}},
			{{0,4,4,0,0},{0,3,3,0,0}},
			{{0,4,4,0,0},{0,3,3,0,0}},
			{{0,4,4,0,0},{0,3,3,0,0}}
		},
		{
			{{0,3,4,0,0},{0,3,4,0,0}},
			{{0,3,3,3,0},{0,3,4,3,0}},
			{{0,0,4,3,0},{0,0,4,3,0}},
			{{0,3,4,3,0},{0,3,3,3,0}}
		},
		{
			{{0,4,3,0,0},{0,3,4,0,0}},
			{{0,2,3,3,0},{0,4,4,3,0}},
			{{0,4,3,0,0},{0,3,4,0,0}},
			{{0,2,3,3,0},{0,4,4,3,0}}
		},
		{
			{{0,3,3,4,0},{0,3,3,3,0}},
			{{0,4,4,0,0},{0,2,4,0,0}},
			{{0,3,3,3,0},{0,4,3,3,0}},
			{{0,4,2,0,0},{0,4,4,0,0}}
		},
		{
			{{0,3,4,0,0},{0,4,3,0,0}},
			{{0,4,4,3,0},{0,2,3,3,0}},
			{{0,3,4,0,0},{0,4,3,0,0}},
			{{0,4,4,3,0},{0,2,3,3,0}}
		},
	};
	void init(){
		srand(_rdtsc());
		type=rand()%7;
		srand(_rdtsc());
		dir=rand()%4;
		x=3;
		y=0-dis[type][dir][0];
		return;
	}
	bool move(){
		paint();
		bool flag=false;
		if(y+5-dis[type][dir][2]>=20) flag=true;
		for(int i=0;i<5-dis[type][dir][1]-dis[type][dir][3];i++){
			if(g[x+i+dis[type][dir][3]][y+hit[type][dir][i+dis[type][dir][3]]]==1) flag=true;
		}
		if(flag){
			int cnt=0;
			int cur[2][10]={0};
			for(int i=0;i<5;i++)for(int j=0;j<5;j++){
				int tmp=j*5+i;
				if(shape[4*type+dir][tmp]==1){
					cur[0][cnt]=i;
					cur[1][cnt]=j;
					cnt++;
				}
			}
			while(cnt--){
				g[x+cur[0][cnt]][y+cur[1][cnt]]=1;
			}
			PaintMap();
			return false;
		}
		y++;
		return true;
	}
	void paint(){
		//Paint Game Map
		int cnt=0;
		int cur[2][10]={0};
		for(int i=0;i<5;i++)for(int j=0;j<5;j++){
			int tmp=j*5+i;
			if(shape[4*type+dir][tmp]==1){
				cur[0][cnt]=i;
				cur[1][cnt]=j;
				cnt++;
			}
		}
		while(cnt--)PaintGrid((x+cur[0][cnt])*20+20,(y+cur[1][cnt])*20+20,20,true);
	}
}next;
queue<Grid> q;

void PaintInterface(){
	char tmp[15];
	setcolor(WHITE);
	rectangle(20,20,580,420);
	rectangle(20,20,220,420);
	snprintf(tmp,15,"Score: %c%c%c%c",Score/1000%10+'0',Score/100%10+'0',Score/10%10+'0',Score%10+'0');
	settextstyle(20,0,"./tahomabd.ttf");
	settextcolor(WHITE);
	settextstyle(15,0,"./tahomabd.ttf");
	outtextxy(360,40,tmp);
	// next graph show
	rectangle(340,100,480,240);
	settextcolor(WHITE);
	outtextxy(360,80,"Next:");
	int cnt=0;
	int cur[2][10]={0};
	for(int i=0;i<5;i++){
		for(int j=0;j<5;j++){
			if(next.shape[next.type*4+next.dir][j*5+i]){
				cur[0][cnt]=i;
				cur[1][cnt]=j;
				cnt++;
			}
		}
	}
	while(cnt--)PaintGrid(cur[0][cnt]*20+20+340,cur[1][cnt]*20+20+100,20,true);
	return;
}

void PaintMap(){
	// 如果行填满则删除该行 
	bool clear[25];
	memset(clear,true,sizeof(clear));
	for(int j=0;j<20;j++){
		for(int i=0;i<10;i++){
			if(g[i][j]!=1) clear[j]=false;
			if(g[i][0]==1) GameOver=true;
		}
	}
	for(int i=1;i<20;i++){
		if(clear[i]){
			Score+=5;
			for(int j=i;j>=0;j--){
				for(int k=0;k<10;k++){
					g[k][j]=g[k][j-1];
				}
			}
		}
	}
	//Paint Graph
	int cnt=0,cur[2][500]={0};
	for(int j=0;j<400/20;j++){
		for(int i=0;i<200/20;i++){
			if(g[i][j]==1){
				cur[0][cnt]=i;
				cur[1][cnt]=j;
				cnt++;
			}else{
				PaintGrid(i*20+20,j*20+20,20,false);
			}
		}
	}
	while(cnt--)PaintGrid(cur[0][cnt]*20+20,cur[1][cnt]*20+20,20,true);
	return;
}

void TickPlayer(){
	if(key_down('W')){
		bool flag=false;
		q.front().dir++;
		q.front().dir%=4;
		// 地图边界碰撞 
		if(q.front().x+5-q.front().dis[q.front().type][q.front().dir][1]>10) flag=true;
		if(q.front().x+q.front().dis[q.front().type][q.front().dir][3]<0) flag=true;
		// 方格碰撞 
		for(int i=q.front().dis[q.front().type][q.front().dir][0];i<5-q.front().dis[q.front().type][q.front().dir][2];i++){
			if(g[q.front().x+5-q.front().lrhit[q.front().type][q.front().dir][0][i-q.front().dis[q.front().type][q.front().dir][0]]][q.front().y+i-q.front().dis[q.front().type][q.front().dir][0]]==1) flag=true;
		}
		for(int i=q.front().dis[q.front().type][q.front().dir][0];i<5-q.front().dis[q.front().type][q.front().dir][2];i++){
			if(g[q.front().x+q.front().lrhit[q.front().type][q.front().dir][1][i-q.front().dis[q.front().type][q.front().dir][0]]-1][q.front().y+i-q.front().dis[q.front().type][q.front().dir][0]]==1) flag=true;
		}
		//回到上一个方向 
		if(flag) q.front().dir+=3,q.front().dir%=4;
		while(key_down('W'));
	}
	if(key_down('A')){
		q.front().x--;
		// 方块碰撞 
		for(int i=q.front().dis[q.front().type][q.front().dir][0];i<5-q.front().dis[q.front().type][q.front().dir][2];i++){
			if(g[q.front().x+5-q.front().lrhit[q.front().type][q.front().dir][0][i-q.front().dis[q.front().type][q.front().dir][0]]][q.front().y+i-q.front().dis[q.front().type][q.front().dir][0]]==1) q.front().x++;
		}
		q.front().x=max(q.front().x,0-q.front().dis[q.front().type][q.front().dir][3]);		//边界碰撞 
		Sleep(150);
	}
	if(key_down('D')){
		q.front().x++;
		// 方块碰撞 
		for(int i=q.front().dis[q.front().type][q.front().dir][0];i<5-q.front().dis[q.front().type][q.front().dir][2];i++){
			if(g[q.front().x+q.front().lrhit[q.front().type][q.front().dir][1][i-q.front().dis[q.front().type][q.front().dir][0]]-1][q.front().y+i-q.front().dis[q.front().type][q.front().dir][0]]==1) q.front().x--;
		}
//		if(flag)
		
		q.front().x=min(q.front().x,10-5+q.front().dis[q.front().type][q.front().dir][1]);		//边界碰撞 
		Sleep(150);
	}
	if(key_down('S')){
		timer=0;
		Sleep(40);
	}
	if(key_down(13)){
		while(q.front().move());
		timer=0;
		while(key_down(13));
	}
	return;
}
//玩家操作进程 
DWORD WINAPI PlayerThread(LPVOID lpParameter){
	while(!GameOver){
		TickPlayer();
	}
	return 0;
}
int main(){
	//Set window tile and size 
	system("title Tetris");
	system("mode con cols=80 lines=20");
	// Main Loop
	while(1){
		Score=0;
		GameOver=false;
		memset(g,false,sizeof(g));
		time_t timep;
		time(&timep);
		char tmp[256];
		system("cls");
		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
		cout<<tmp<<endl;
		Sleep(80);
		cout<<"		_______ _______ _______  ______ _____ _______\n		   |    |______    |    |_____/   |   |______\n		   |    |______    |    |    \\_ __|__ ______|\n\n\n";
		Sleep(80);
		cout<<"						Author: MickyRex\n\n";
		Sleep(40);
		cout<<"		操作说明：\n\n			 W 旋转90度\n			 A 左移\n			 D 右移\n			 S 下落\n			 Enter 直接掉落\n\n		";
		Sleep(500);
		system("pause");
		//Grid-down Loop 
		while(key_down(13));
		initgraph(600,440);
		CreateThread(NULL,0,&PlayerThread,0,0,NULL);
		next.init();
		while(!GameOver){
			//next Grid 
			q.push(next);
			next.init();
			timer=1;
			int rate=80;
			while(1){
				timer%=rate;
				// clear
				BeginBatchDraw();
				cleardevice();
				PaintMap();
				PaintInterface();
				bool flag;
				//Grid down
				if(timer==0) flag=q.front().move();
				q.front().paint();
				//Paint in window
				FlushBatchDraw();
				if(timer==0 and !flag){
					//next
					q.pop();
					break;
				}
				timer++;
			}
		}
		closegraph();
	}
	return 0;
}
