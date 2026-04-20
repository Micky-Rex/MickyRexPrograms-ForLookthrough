/*
	Tetris-2.0 -- By MickyRex
*/
#pragma once
#include <ctime>
#include <queue>
#include <windows.h>

#include "grid_shape.h"
using namespace std;

const int N=20+5,M=10+5;
int score=0,timer=0;
int g[N /* Y */][M /* X */];/* g[y][x] = (x,y) */

int myRand(int MOD){
	srand(_rdtsc());
	return rand()%MOD;
}

struct Grid{
	int type;
	int x,y,dir;
	bool isPut;
	void init(){
		type=myRand(7);
		dir=myRand(4);
		x=3,y=-dis[type*4+dir];
		isPut=false;
//		move(3),move(3);
		return;
	}
	
	bool canMove(){
		bool flag=true;
		for(int i=1;i<=5;i++){
			for(int j=1;j<=5;j++){
				if(i+y>=0 and j+x>=0 and g[i+y][j+x]==1 and shape[type*4+dir][i*5+j-6]){
					flag=false;
				}
			}
		}
		return flag;
	}
	
	void move(int mode /*0 - down, 1 - left, 2 - right, 3 - up*/){
		int pos[]={x,y};
		switch(mode){
			case 0:y++;break;
			case 1:x--;break;
			case 2:x++;break;
			case 3:y--;break;
		}
		if(!canMove()){
			x=pos[0],y=pos[1];
			if(!mode)put();
		}
		
		return;
	}
	
	void rotate(){
		dir=(dir+5)%4;
		if(!canMove()){
			dir=(dir+3)%4;
		}
		return;
	}
	
	void put(){
		dir%=4;
		for(int i=1;i<=5;i++){
			for(int j=1;j<=5;j++){
				int tmp=shape[type*4+dir][i*5+j-6];
				if(tmp==1) g[i+y][j+x]=tmp;
			}
		}
		isPut=true;
		return;
	}
	
};

queue<Grid> grid;

Grid getNext(){
	Grid tmp;
	tmp.init();
	return tmp;
}

void checkQueue(){
	while(grid.size()<2){
		grid.push(getNext());
	}
	if(!grid.empty()){
		if(grid.front().isPut){
			
			grid.pop();
		}
	}else if(grid.size()>2){
		grid.pop();
	}
	return;
}
