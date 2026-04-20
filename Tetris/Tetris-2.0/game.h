/*
	Tetris-2.0 -- By MickyRex
*/
#pragma once
#include <map>
#include <ctime>
#include <stack>
#include <vector>
#include <fstream>
#include <windows.h>
#include <graphics.h>
#include "tetris.h"
#ifndef key_down()
#define key_down(key_name) ((GetAsyncKeyState(key_name))?1:0)
#endif
using namespace std;
bool gameOver=false;

void checkClear(){
	bool clear[20+5],flag;
	for(int i=1;i<=20;i++){
		flag=true;
		for(int j=1;j<=10;j++){
			if(g[i][j]==0){
				flag=false;
				break;
			}
		}
		clear[i]=flag;
	}
	for(int i=1;i<=20;i++){
		if(clear[i]){
			score+=5;
			for(int j=i;j>0;j--){
				for(int k=1;k<=10;k++){
					g[j][k]=g[j-1][k];
				}
			}
		}
	}
	return;
}

void checkOver(){
	for(int i=1;i<=10;i++){
		if(g[1][i]==1){
			gameOver=true;
			break;
		}
	}
	return;
}

void paintGrid(int x,int y,int a,bool show){
	setcolor(show?WHITE:RGB(123,123,123));
	setfillcolor(show?RGB(0,119,230):BLACK);
	fillrectangle(x,y,x+a,y+a);
	return;
}
 
void paintUi(){
	setcolor(WHITE);
	rectangle(20,20,580,420);
	rectangle(240,40,390,190);
	line(220,20,220,420);
	return;
}

void paintMap(){
	// Paint Map Grid
	int cur=0;
	int tmp[2][500];
	for(int i=1;i<=20;i++){
		for(int j=1;j<=10;j++){
			if(!g[i][j])paintGrid(j*20,20*i,20,false);
			else tmp[0][cur]=i,tmp[1][cur]=j,cur++;
		}
	}
	for(int i=0;i<cur;i++)paintGrid(tmp[1][i]*20,20*tmp[0][i],20,true);
	// Paint Next Grid
	Grid temp=grid.back();
	for(int i=0;i<5;i++){
		for(int j=0;j<5;j++){
			if(shape[temp.type*4+temp.dir][i*5+j])paintGrid(j*30+240,i*30+40,30,true);
		}
	}
	return;
}

void tickPlayer(){
	if(key_down('W') or key_down(VK_UP)){
		grid.front().rotate();
		while(key_down('W') or key_down(VK_UP));
	}else if(key_down('A') or key_down(VK_LEFT)){
		grid.front().move(1);
//		while(key_down('A') or key_down(VK_LEFT));
	}else if(key_down('D') or key_down(VK_RIGHT)){
		grid.front().move(2);
//		while(key_down('D') or key_down(VK_RIGHT));
	}else if(key_down('S') or key_down(VK_DOWN)){
		grid.front().move(0);
	}else if(key_down(13)){
		for(int i=0;i<20;i++) grid.front().move(0);
		while(key_down(13));
	}
	return;
}
