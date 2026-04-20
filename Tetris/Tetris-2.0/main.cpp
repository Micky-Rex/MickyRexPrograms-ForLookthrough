/*
	Tetris-2.0 -- By MickyRex
*/
#include "tetris.h"
#include <iostream>
#include <graphics.h>
#include "game.h"
using namespace std;

int timerCounter=0,range=80;
Grid getNext();
void showGrid();
DWORD WINAPI playerThread(LPVOID lpParameter);

int main(){
	
	system("title Tetris-2.0");
	initgraph(600,440);
	while(1){
		timer=0;
		cleardevice();
		memset(g,0,sizeof(g));
		while(!grid.empty())grid.pop();
		// Set map pale
		for(int i=1;i<=20;i++)g[i][11]=1;
		for(int i=1;i<=10;i++)g[21][i]=1;
		for(int i=1;i<=20;i++)g[i][0]=1;
		CreateThread(NULL,0,&playerThread,0,0,NULL);
		gameOver=false;
		checkQueue();
		while(!gameOver){
			if(timerCounter==0){
				grid.front().move(0);
				checkClear();
				checkOver();
			}
			BeginBatchDraw();
			cleardevice();
			checkQueue();
			paintMap();
			paintUi();
			showGrid();
			FlushBatchDraw();
			timerCounter++;
			timerCounter%=range;
		}
		Sleep(500);
		BeginBatchDraw();
		cleardevice();
		FlushBatchDraw();
		Sleep(500);
	}
	
	return 0;
}

void showGrid(){
	for(int i=0;i<5;i++){
		for(int j=0;j<5;j++){
			Grid tmp=grid.front();
			if(shape[tmp.type*4+tmp.dir][i*5+j]==1) paintGrid((tmp.x+j)*20+20,(tmp.y+i)*20+20,20,true);
		}
	}
	return;
}

DWORD WINAPI playerThread(LPVOID lpParameter){
	while(!gameOver){
		tickPlayer();
		Sleep(80);
	}
	return 0;
}
