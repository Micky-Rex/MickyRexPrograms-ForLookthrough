#include<ctime>
#include<queue>
#include<conio.h>
#include<cstdlib>
#include<iostream>
#include<graphics.h>
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)	//��������̰����ĵ���¼� 
#define KEY_DOWN(VK_NINAME)((GetAsyncKeyState(VK_NINAME)&0x8000)?1:0)  //��������갴���ĵ���¼� 
using namespace std;
const int N=106;	//�������߳� 
int mine_num=0,open=0,starttime=time(NULL);
int zoom=2;
int picturex=21,picturey=21;
int n,m;	//��ͼ�Ŀ��� 
int window_width;	//���ڿ��� 
int window_height;	//���ڸ߶� 
int g[N][N];	//��¼ɨ�׵�ͼ  
int menuh=20;
bool vis[N][N];	//��¼�Ƿ�� 
bool mapflag[N][N];
bool win_flag=true;
POINT p;
IMAGE p0,p1,p2,p3,p4,p5,p6,p7,p8,pm,pn,pnd,pf; 	//�洢ͼƬ�ز� 

void load_picture();	//����ͼƬ�ز� 
void around();	//�����Χ���� 
int flag_mine(int x,int y);
void paint();
void mapdfs(int x,int y);
//void mapbfs(int x,int y);

int main(){
	bool selfleg=false;
	while(!selfleg){
		if(MessageBox(GetForegroundWindow(),"10x10?","Map size",4)==6){	//�� 
			n=10,m=10;
			picturex=21,picturey=21;
			zoom=2;
			window_width=n*(picturex*zoom);	//���ڿ��� 
			window_height=m*(picturey*zoom);	//���ڸ߶� 
			selfleg=true;
		}else if(MessageBox(GetForegroundWindow(),"40*40?","Map size",4)==6){
			n=40,m=40;
			picturex=10,picturey=10;
			zoom=2;
			window_width=n*(picturex*zoom);	//���ڿ��� 
			window_height=m*(picturey*zoom);	//���ڸ߶� 
			selfleg=true;
		}else if(MessageBox(GetForegroundWindow(),"40*90?","Map size",1)==1){
			n=90,m=40;
			picturex=10,picturey=10;
			zoom=2;
			window_width=n*(picturex*zoom);	//���ڿ��� 
			window_height=m*(picturey*zoom);	//���ڸ߶� 
			selfleg=true;
		}
	}
	memset(vis,false,sizeof(vis));
	memset(mapflag,false,sizeof(mapflag));
	win_flag=true;
	initgraph(window_width,window_height+menuh);
	system("title MineSweeper");
	srand(std::time(0));
	load_picture();
	around();
	while(true){
		setbkcolor(WHITE); 
//		˫�������� 
		BeginBatchDraw();
		cleardevice();
		paint();
		settextcolor(BLACK);
		settextstyle(menuh-2,0,"./STXINWEI.TTF");
		outtextxy(0,0," ������");
		outtextxy(55,0,mine_num/100%10+'0');
		outtextxy(65,0,mine_num/10%10+'0');
		outtextxy(75,0,mine_num%10+'0');
		if(!win_flag){
			FlushBatchDraw();
			cout<<'\a';
			if(MessageBox(GetForegroundWindow(),"GameOver","MineSweeper",5)!=4)break;
			memset(vis,false,sizeof(vis));
			memset(mapflag,false,sizeof(mapflag));
			win_flag=true;
			open=0;
		}else if(open==n*m-mine_num){
			FlushBatchDraw();
			cout<<'\a';
			MessageBox(GetForegroundWindow(),"Victory!","MineSweeper",1);
			break;
		}
		FlushBatchDraw();
	}
	closegraph();
	return 0;
}

void paint(){
	bool flag;	//����Ƿ���ͣ�ڸ÷���ı�Ǳ��� 
	for(int i=0;i<window_width;i+=(picturex*zoom)){
		for(int j=0;j<window_height;j+=(picturey*zoom)){
			flag=false;
			j+=menuh;
			GetCursorPos(&p);
			ScreenToClient(GetForegroundWindow(),&p);
			if(p.x>i and p.x<i+(picturex*zoom) and p.y>j and p.y<j+(picturey*zoom)) flag=true;	//�����ͣ�ڷ�����ʱ 
			if(KEY_DOWN(VK_LBUTTON)){
				if(p.x>i and p.x<i+(picturex*zoom) and p.y>j and p.y<j+(picturey*zoom)){
					if(!mapflag[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)])mapdfs(i/(picturex*zoom),(j-menuh)/(picturey*zoom));	//�����������չ�� 
					if(g[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)]==-1 and vis[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)]) win_flag=false;
					while(KEY_DOWN(VK_LBUTTON));
				}
			}
			if(KEY_DOWN(VK_RBUTTON)){
				if(p.x>i and p.x<i+(picturex*zoom) and p.y>j and p.y<j+(picturey*zoom)){
					mapflag[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)]=true-mapflag[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)];
					while(KEY_DOWN(VK_RBUTTON));
				}
			}
			j-=menuh;
			if(vis[i/(picturex*zoom)][j/(picturey*zoom)]){
				//�� i , j ������ͼ
				int gridnum=g[i/(picturex*zoom)][j/(picturey*zoom)];
				j+=menuh;
				switch(gridnum){
					case -1:putimage(i,j,&pm);break;
					case 0:	putimage(i,j,&p0);break;
					case 1:	putimage(i,j,&p1);break;
					case 2:	putimage(i,j,&p2);break;
					case 3:	putimage(i,j,&p3);break;
					case 4:	putimage(i,j,&p4);break;
					case 5:	putimage(i,j,&p5);break;
					case 6:	putimage(i,j,&p6);break;
					case 7:	putimage(i,j,&p7);break;
					case 8:	putimage(i,j,&p8);break;
				}
				j-=menuh;
			}else{
				j+=menuh;
				if(flag) putimage(i,j,&pnd);
				else putimage(i,j,&pn);
				if(mapflag[i/(picturex*zoom)][(j-menuh)/(picturey*zoom)]) putimage(i,j,&pf);
				j-=menuh;
			}
		}
	}
	return;
}

void around(){
	//�����ͼ
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			srand(__rdtsc());
			if(rand()%100<10){
				g[i][j]=-1;
				mine_num++;
				if(i==0 and j==0)g[i][j]=0;
				if(i==n-1 and j==0)g[i][j]=0;
				if(i==0 and j==m-1)g[i][j]=0;
				if(i==n-1 and j==m-1)g[i][j]=0;
			}
		}
	}
	//�����з���λ����������������Χ����
	for(int i=0;i<n;i++) for(int j=0;j<m;j++) if(g[i][j]!=-1) g[i][j]=flag_mine(i,j);
	return;
}

void mapdfs(int x,int y){
	//�ݹ���� 1
	if(vis[x][y]==false) open++,vis[x][y]=true;
	else return;
	//�ݹ���� 2
	if(g[x][y]!=0) return;
	//�����ܵݹ� 
	if(x>0)mapdfs(x-1,y);
	if(y>0)mapdfs(x,y-1);
	if(x<n-1)mapdfs(x+1,y);
	if(y<m-1)mapdfs(x,y+1);
}

int flag_mine(int x,int y){
	const int dx[]={1,1,1,0,0,-1,-1,-1};
	const int dy[]={-1,0,1,-1,1,-1,0,1};
	//������Χ 8�����񣬱������
	int nx,ny;
	int ans=0;
	for(int i=0;i<8;i++){
		nx=x+dx[i];
		ny=y+dy[i];
		if(nx<0 or ny<0 or nx>n-1 or ny>m-1) continue;
		if(g[nx][ny]==-1) ans++;
	}
	return ans;
}


void load_picture(){
	memset(vis,false,sizeof(vis));
	memset(mapflag,false,sizeof(mapflag));
	int px=picturex+1,py=picturey+1;
	int w=px*zoom,h=py*zoom;	//        ����������
	//����ͼƬ
	loadimage(&p0,_T("./Picture/0.png"),w,h);
	loadimage(&p1,_T("./Picture/1.png"),w,h);
	loadimage(&p2,_T("./Picture/2.png"),w,h);
	loadimage(&p3,_T("./Picture/3.png"),w,h);
	loadimage(&p4,_T("./Picture/4.png"),w,h);
	loadimage(&p5,_T("./Picture/5.png"),w,h);
	loadimage(&p6,_T("./Picture/6.png"),w,h);
	loadimage(&p7,_T("./Picture/7.png"),w,h);
	loadimage(&p8,_T("./Picture/8.png"),w,h);
	loadimage(&pm,_T("./Picture/mine.png"),w,h);
	loadimage(&pf,_T("./Picture/flag.png"),w,h);
	loadimage(&pn,_T("./Picture/unexplored.png"),w,h);
	loadimage(&pnd,_T("./Picture/unexplored2.png"),w,h);
	return;
}
