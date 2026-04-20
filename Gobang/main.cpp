#include<vector>
#include<stdio.h>
#include<graphics.h>
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)
#define KEY_DOWN(VK_NINAME)((GetAsyncKeyState(VK_NINAME)&0x8000)?1:0)
using namespace std;
bool next_color=1;
POINT MOUSE;
POINT MOUSE_GRID;
char MAP[30][30];
struct PIECE {
	int x,y;
	bool color;
	void init() {
		x=MOUSE_GRID.x,y=MOUSE_GRID.y;
		color=next_color;// x*=50
		next_color=next_color?false:true;
		MAP[x][y]=(color?'1':'0');
		return;
	}
} next;

void get_mouse_pos();
void paint_pieces();
void tick_player();
char check_win();

int main()
{
	char winner;
	IMAGE board;
	loadimage(&board,"./Board.png",1000,1000);
	memset(MAP,0,sizeof(MAP));
	initgraph(1000,1000);
	while(1) {
		get_mouse_pos();
		tick_player();
		BeginBatchDraw();
		cleardevice();
		putimage(0,0,&board);
		paint_pieces();
		winner=check_win();
		FlushBatchDraw();
		if(winner!=0) {
			goto game_over;
		}
	}
game_over:
	;
	closegraph();
	if(winner != 'T') printf("%s PIECE WIN !!!\n\n\n",winner=='1'?"BLACK":"WHITE");
	else printf("DRAW !!!\n");
	system("pause");
	return 0;
}

void paint_pieces()
{
	for(int i=1; i<20; i++) {
		for(int j=1; j<20; j++) {
			if(MAP[i][j]!=0) {
				setcolor(MAP[i][j]=='1'?BLACK:WHITE);
				setfillcolor(MAP[i][j]=='1'?BLACK:WHITE);
				fillcircle(i*50,j*50,18);
			}
		}
	}
	setcolor(MAP[MOUSE_GRID.x][MOUSE_GRID.y]==0?(next_color?BLACK:WHITE):RED);
	circle(MOUSE_GRID.x*50,MOUSE_GRID.y*50,18);
	return;
}

void tick_player()
{
	if(KEY_DOWN(VK_LBUTTON)) {
		if(MAP[MOUSE_GRID.x][MOUSE_GRID.y]==0)
			if(MOUSE_GRID.x>=1 and MOUSE_GRID.x<=19 and MOUSE_GRID.y>=1 and MOUSE_GRID.y<=19)
				next.init();
		while(KEY_DOWN(VK_LBUTTON));
	}
	return;
}

void get_mouse_pos()
{
	GetCursorPos(&MOUSE);
	ScreenToClient(GetForegroundWindow(),&MOUSE);
	MOUSE_GRID.x=(MOUSE.x+25)/50;
	MOUSE_GRID.y=(MOUSE.y+25)/50;
	return;
}

char check_win()
{
	int x,y;
	char now,winner=0;
	bool flag=false;
	const int dx[]= {-1,0,1,-1,1,-1,0,1};
	const int dy[]= {-1,-1,-1,0,0,1,1,1};
	for(int i=1; i<20; i++) {
		for(int j=1; j<20; j++) {
			now=MAP[i][j];
			if(now==0)continue;
			for(int k=0; k<8; k++) {
				flag=true;
				for(int l=0; l<5; l++) {
					if(i+dx[k]*l<1 or i+dx[k]*l>19 or j+dy[k]*l<1 or j+dy[k]*l>19)flag=false;
					if(MAP[i+dx[k]*l][j+dy[k]*l]!=now)flag=false;
				}
				if(flag) {
					winner=now;
					goto end;
				}
			}
		}
	}
	flag=false;
	for(int i=1; i<20; i++) {
		for(int j=1; j<20; j++) {
			if(MAP[i][j]==0) flag=true;
		}
	}
	if(!flag)
		return 'T';
	end:;
	return winner;
}
