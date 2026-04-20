
#include <set>
#include <map>
#include <list>
#include <thread>
#include <chrono>
#include <math.h>
#include <string>
#include <vector>
#include <conio.h>
#include <stdio.h>
#include <random>
#include <WinSock2.h>
#include <graphics.h>

#include "myMath.h"
#include "Input.h"
#include "AABB.h"

#ifndef SNAKE_H
#define SNAKE_H
#define MAX_CLIENT 127
#define MAX_BUF_SIZE 4096
#define MAX_TEXTURES 100
#define _CRT_SECURE_NO_WARINGS

using namespace std;
int windowW,windowH;
WSADATA wsd;
float FPS = 60.0f;
int64_t frameDelayNs = static_cast<int64_t>(1000.0f / FPS * 1e6);
const int PORT = 8000;
long long unsigned int NOW_TIME;

float cameraX,cameraY;
float zoomFactor = 2.0f;

char recvBuf[MAX_BUF_SIZE];
char sendBuf[MAX_BUF_SIZE];

void getTime()
{
	auto now = chrono::system_clock::now();
	auto duration_since_epoch = now.time_since_epoch();
	auto millis = chrono::duration_cast<chrono::milliseconds>(duration_since_epoch);
	NOW_TIME = millis.count();
	return;
}

struct FOOD
{
	float x,y;
	int score,size;
	COLORREF color;
	void draw()
	{
		setcolor(BLACK);
		setfillcolor(color);
		fillcircle(x*zoomFactor - cameraX*zoomFactor + windowW/2,y*zoomFactor - cameraY*zoomFactor + windowH/2,size);
		return;
	}
};

vector<FOOD> foods;

void initFood(int n)
{
	FOOD food;
	default_random_engine ran;
	while(n--)
	{
		food.x = ran() % 1000;
		food.y = ran() % 1000;
		food.size = 8;
		food.score = 4;
		food.color = RGB(ran()%255,ran()%255,ran()%255);
		foods.push_back(food);
		printf("%.1f, %.1f\n",food.x,food.y);
	}
	return;
}

struct BODY
{
	myCIRCLE aabb;
	float x,y;
	BODY(float _x,float _y,float _size)
	{
		x = _x;
		y = _y;
		aabb.radians = _size;
		return;
	}
	void updateAABB()
	{
		aabb.pos = POINT({static_cast<LONG>(x),static_cast<LONG>(y)});
		return;
	}
};

struct PLAYER
{
private:
	bool invincible;
	int bodyLen;
	float dir,speed;
	int len,paintDis;
	vector<BODY> snake;
	long long int BURN_TIME;
public:
	void init(float x, float y, int _len)
	{
		speed = 4;
		len = _len;
		paintDis = 4;
		bodyLen = len * paintDis;
		getTime();
		BURN_TIME = NOW_TIME;
		invincible = true;
		for(register int i = 0; i < bodyLen; i++)
		{
			snake.push_back(BODY(x,y,10));
		}
		return;
	}
	
	void update(int ox,int oy)
	{
		// Angle clamp
		float change = angleToXAxis(ox,oy,mouse.x,mouse.y);
		if(abs(change - dir) <= M_PI)
		{
			dir += clampf(change - dir, _degToRad(speed * 4), _degToRad(-speed * 4));
		}
		else
		{
			if(dir < M_PI) dir -= clampf(dir + M_PI * 2 - change, _degToRad(speed * 4), _degToRad(-speed * 4));
			else dir -= clampf(change - dir, _degToRad(speed * 4), _degToRad(-speed * 4));
		}
		if(dir < 0) dir += M_PI * 2;
		if(dir > M_PI * 2) dir -= M_PI * 2;
		
		// Speed up
		if(KEY_DOWN(VK_LBUTTON))
		{
			move();
		}
	
		return;
	}
	
	void move()
	{
		if(snake.size() < 2) return;
		snake.insert(snake.begin(),
			BODY(
				static_cast<float>(snake.front().x + cos(dir) * speed),
				static_cast<float>(snake.front().y - sin(dir) * speed), snake.front().aabb.radians) );
		
		if(snake.size() < bodyLen)
		{
			snake.push_back(BODY(snake.back().x,snake.back().y,snake.back().aabb.radians));
		}
		while(snake.size() > bodyLen)
		{
			snake.pop_back();
		}
		return;
	}
	
	inline bool checkDie()
	{
		if(invincible)
		{
			getTime();
			if(NOW_TIME - BURN_TIME > 1000) invincible = false;	// 出生无敌时间 
			return false;
		}
		
//		BeginBatchDraw();
//		cleardevice();
		
		snake[paintDis].updateAABB();
		for(register int i = paintDis * 4 - 1; i < snake.size(); i += paintDis)
		{
			setcolor(WHITE);
			snake[i].updateAABB();
//			circle(snake[i].aabb.pos.x - cameraX + windowW/2, snake[i].aabb.pos.y - cameraY + windowH/2, snake[i].aabb.radians);
			if(isContact(snake[paintDis].aabb, snake[i].aabb))
			{
				printf("%d\n",i);
				return true;
			}
		}
//		FlushBatchDraw();
		return false;
	}
	
	void checkEat()
	{
		register int i = 0;
		for(i = foods.size() - 1; i >= 0; i--)
		{
			FOOD cur = foods[i];
			if(__distance(cur.x, cur.y, snake.front().x, snake.front().y) <= snake.front().aabb.radians + cur.size)
			{
				bodyLen += cur.score;
				foods.erase(foods.begin() + i);
				i--;
			}
		}
//		printf("1");
//		while(!del.empty())
//		{
//			foods.erase(foods.begin() + *del.rbegin());
//			printf("%d, ",*del.rbegin());
//			del.erase(prev(del.end()));
//		}
		len = bodyLen / paintDis;
		
		return;
	}
	
	inline void draw()
	{
		register int i;
		setcolor(BLACK);
		setfillcolor(RED);
		for(i = snake.size() - snake.size()%paintDis - 1; i >= 0; i -= paintDis)
		{
			if(	(snake[i].x - cameraX) * zoomFactor + windowW/2 < -10 * zoomFactor or (snake[i].x - cameraX) * zoomFactor + windowW/2 > (windowW + 10) * zoomFactor or
				(snake[i].y - cameraY) * zoomFactor + windowH/2 < -10 * zoomFactor or (snake[i].y - cameraY) * zoomFactor + windowH/2 > (windowH + 10) * zoomFactor)
			{
				continue;
			}
			fillcircle((snake[i].x - cameraX) * zoomFactor + windowW/2, (snake[i].y - cameraY) * zoomFactor + windowH/2, snake[i].aabb.radians * zoomFactor);
		}
		return;
	}
	
	pair<float, float> getHeadPos()
	{
		return make_pair(snake.front().x, snake.front().y);
	}
};

map<SOCKET, PLAYER> players;
#endif
