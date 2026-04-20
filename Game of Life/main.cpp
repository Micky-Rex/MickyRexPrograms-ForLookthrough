#include<conio.h>
#include<graphics.h>
#include<bits/stdc++.h>
//#define WM_MOUSEWHEEL=0x020A
#define KEY_DOWN(VK_NINAME)((GetAsyncKeyState(VK_NINAME)&0x8000)?1:0)  //用来检测按键的点击事件 
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)

int mx,my;
double zoom=1.5,Z=1.5;
int camx=0,camy=0,rectx,recty,nowx,nowy;
const int side=10*zoom;
const int N=1000,M=1000;
const int windoww=960,windowh=540;
const int n=windoww/side,m=windowh/side;
int g[N][M]={0};
bool vis[N][M]={0};
bool should_init_map = false;
IMAGE GRID0,GRID1,menu;
POINT p;
HANDLE houtput=GetStdHandle(STD_OUTPUT_HANDLE);   //用来获取窗口的输出句柄，将它储存到houtput 
HWND h=GetForegroundWindow();  //获取最顶层窗口的句柄 

void set();
void tick();
void paint();
void mouse();
void editor();
void camera();
void around();
void keyboard();
void init_map(double);
void movecamera(bool f);
void loadpicture();
void initialization();

void build(int n,int x,int y){
	switch(n){
		case 1:
			//滑翔机 
			vis[x][y+1]=1;
			vis[x+1][y+2]=1;
			vis[x+2][y]=1;
			vis[x+2][y+1]=1;
			vis[x+2][y+2]=1;
			break;
		case 2:
			//轻型飞船 
			vis[x][y+3]=1;
			vis[x+1][y+4]=1;
			vis[x+2][y+4]=1;
			vis[x+3][y+4]=1;
			vis[x+3][y+3]=1;
			vis[x+3][y+2]=1;
			vis[x+3][y+1]=1;
			vis[x+2][y]=1;
			break;
		case 3:
			//高斯帕机枪 
			
			break;
		case 4:
			//脉冲星
			break;
		case 5:
			//不倒翁
			break;
		case 6:
			//高克星系
			break; 
	}
	return;
}

int main()
{
	initialization();
	while(true)
	{
		BeginBatchDraw();
		cleardevice();
		camera();
		editor();
		tick();
		paint();
		Sleep(3);
		FlushBatchDraw();
	}
	return 0;
}

void initialization(){
	memset(vis,false,sizeof(vis));
	loadpicture();
	initgraph(windoww,windowh,0);
	return;
}

void mouse()
{
	h=GetForegroundWindow();
	GetCursorPos(&p);
	ScreenToClient(h,&p);
	return;
}

void set()
{
	//限制摄像头 
	camx=std::max(0,camx);
	camx=std::min(side*N-side*n,camx);
	camy=std::max(0,camy);
	camy=std::min(side*M-side*m,camy);
	return;
}

void camera()
{
	keyboard();
	movecamera(true);
	return;
}

void movecamera(bool f)
{
	//鼠标操纵 
	mouse();
	int cx=camx,cy=camy,mx,my;
	mx=p.x;
	my=p.y;
	while(KEY_DOWN(VK_LBUTTON))
	{
		BeginBatchDraw();
		cleardevice();
		mouse();
		camx=cx-p.x+mx;
		camy=cy-p.y+my;
		set();
		if(f) tick();
		paint();
		FlushBatchDraw();
		
	}
	return;
}
	
void keyboard()
{
	//键盘操纵 
	mouse();
	bool flag=false;
	int x=(p.y+camy)/side,y=(p.x+camx)/side;
	int camspeed=36*side/10;
	if(key_down('w'))camy-=camspeed;
	if(key_down('s'))camy+=camspeed;
	if(key_down('a'))camx-=camspeed;
	if(key_down('d'))camx+=camspeed;
	if(key_down('r'))init_map(0.01);
	if(key_down('1'))build(1,x,y),flag=true;
	else if(key_down('2'))build(2,x,y),flag=true;
	else if(key_down('3'))build(3,x,y),flag=true;
	else if(key_down('4'))build(4,x,y),flag=true;
	else if(key_down('5'))build(5,x,y),flag=true;
	else if(key_down('6'))build(6,x,y),flag=true;
	else if(key_down('7'))build(7,x,y),flag=true;
	else if(key_down('8'))build(8,x,y),flag=true;
	else if(key_down('9'))build(9,x,y),flag=true;
	while(flag){
		if(key_down('1'))build(1,x,y),flag=true;
		else if(key_down('2'))build(2,x,y),flag=true;
		else if(key_down('3'))build(3,x,y),flag=true;
		else if(key_down('4'))build(4,x,y),flag=true;
		else if(key_down('5'))build(5,x,y),flag=true;
		else if(key_down('6'))build(6,x,y),flag=true;
		else if(key_down('7'))build(7,x,y),flag=true;
		else if(key_down('8'))build(8,x,y),flag=true;
		else if(key_down('9'))build(9,x,y),flag=true;
		else flag=false;
	}
	return;
}

void editor()
{
	int cx=camx,cy=camy,mx,my;
	mx=p.x;
	my=p.y;
	//地图编辑 
	while(key_down(' '))
	{
		mouse();
		if(KEY_DOWN(VK_RBUTTON))
		{
			vis[(p.y+camy)/side][(p.x+camx)/side]=true-vis[(p.y+camy)/side][(p.x+camx)/side];
			BeginBatchDraw();
			cleardevice();
			paint();
			FlushBatchDraw();
			while(KEY_DOWN(VK_RBUTTON));
		}else if(KEY_DOWN(VK_LBUTTON))
		{
			movecamera(false);
		}
		
	}
	if(key_down('R'))
	{
		init_map(0.3);
		should_init_map = false;
		while(key_down('R'))
		{
			movecamera(false);
			BeginBatchDraw();
			cleardevice();
			paint();
			FlushBatchDraw(); 
		}
	}
	return;
}


void tick()
{
	around();
	for(int i=0;i<M;i++)
	{
		for(int j=0;j<N;j++)
		{
			if(g[i][j]<2) vis[i][j]=false;
			if(g[i][j]>3) vis[i][j]=false;
			if(g[i][j]==3) vis[i][j]=true;
		}
	}
	return;
}

void around()
{
	const int dx[]={0,0,1,1,1,-1,-1,-1};
	const int dy[]={1,-1,-1,0,1,-1,0,1};
	int nx,ny,ans=0;
	for(int x=0;x<M;x++)
	{
		for(int y=0;y<N;y++)
		{
			ans=0;
			for(int i=0;i<8;i++)
			{
				nx=x+dx[i];
				ny=y+dy[i];
				if(nx<0 or ny<0 or nx>N-1 or ny>M-1) continue;
				if(vis[nx][ny]) ans++;
			}
			g[x][y]=ans;
		}
	}
	return;
}

void paint()
{
	//绘制画面 
	int x=0-camx%side,y=0-camy%side;
	for(int i=0;i<m;i++)
	{
		putimage(x-side,y,&GRID0);
		y+=side;
	}
	y=0-(camy%side);
	for(int i=camy/side;i<m+2+camy/side;i++)
	{
		x=0-camx%side;
		for(int j=camx/side;j<n+2+camx/side;j++)
		{
			if(vis[i][j])putimage(x,y,&GRID1);
			else putimage(x,y,&GRID0);
			x+=side;
		}
		y+=side;
	}
	//绘制菜单
//	putimage(0,0,&menu);
	return;
}

void loadpicture()
{
	loadimage(&GRID0,"./Picture/GRID0.png",side,side,0);
	loadimage(&GRID1,"./Picture/GRID1.png",side,side,0);
//	loadimage(&menu,"./Picture/menu.png",windoww,30,0);
	return;
}

void init_map(double x)
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<M;j++)
		{
			vis[i][j]=(rand()%100<=x*100?true:false);
//			vis[i][j]=true;
		}
	}
	return;
}
