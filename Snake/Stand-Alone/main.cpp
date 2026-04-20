#include "snake.h"
#include "AABB.h"
#include "shape.h"
#include "myMath.h"
#include "button.h"
#include "entryBox.h"
#include "myImages.h"
#include "clientSocket.h"
#include "serverSocket.h"
#pragma comment(dll,"ws2_32.dll")

#define EXIT_GAME 0
#define IN_MAIN_MENU 1
#define IN_JOIN_MENU 2
#define IS_SERVER 100
#define IS_CLIENT 101

void init();
void paint();
void tickGame();
void updateInput();

int buttonNum;
int entryBoxNum;
BUTTON buttons[10];
bool startGame = false;
ENTRY_BOX entryBoxes[10];

int MAIN_MENU();
int JOIN_MENU();

using namespace chrono;

int main()
{
	int menuState = IN_MAIN_MENU;
	init();
	
	// Menus
	while(true)
	{
		if(menuState == IN_MAIN_MENU)
		{
			menuState = MAIN_MENU();
		}
		else if(menuState == IN_JOIN_MENU)
		{
			menuState = JOIN_MENU();
		}
		else if(menuState == IS_SERVER) 
		{
			runGameServer();
			menuState = IN_MAIN_MENU;
		}
		else if(menuState == IS_CLIENT)
		{
//			if(connectServer(entryBoxes[0].text) == 0)
//			{
//				break;
//			}
			break;
			printf("Connect to server failed!\n");
			menuState = IN_JOIN_MENU;
		}
		else if(menuState == EXIT_GAME)
		{
			printf("====EXIT-Game====\n");
			closegraph();
			system("pause");
			exit(0);
		}
		while(KEY_DOWN(VK_LBUTTON));
	}
	
	players.clear();
	players[0].init(500,500,50);
	startGame = true;
	auto start = high_resolution_clock::now();
	buttonNum = 0;
	entryBoxNum = 0;
	initFood(100);
	// Game
	while(true)
	{
		updateInput();
		tickGame();
		paint();
		
		auto now = high_resolution_clock::now();
		auto elapsed_time = duration_cast<nanoseconds>(now - start).count();
		
		if(elapsed_time < frameDelayNs)
		{
			nanoseconds sleep_duration(frameDelayNs - elapsed_time);
			this_thread::sleep_for(sleep_duration);
		}
		start = high_resolution_clock::now();
	}
	getchar();
	closegraph();
	return 0;
}

int MAIN_MENU()
{
	entryBoxNum = 0;
	buttons[0].center = true;
	buttons[0].focusColor = RGB(60,60,60);
	buttons[0].rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3}), POINT({windowW/2+200,windowH/3+40})});
	buttons[0].text = "Host";
	buttons[0].textColor = WHITE;
	buttons[1].center = true;
	buttons[1].focusColor = RGB(60,60,60);
	buttons[1].rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3+60}), POINT({windowW/2+200,windowH/3+100})});
	buttons[1].text = "Join";
	buttons[1].textColor = WHITE;
	buttons[2].center = true;
	buttons[2].focusColor = RGB(60,60,60);
	buttons[2].rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3+120}), POINT({windowW/2+200,windowH/3+160})});
	buttons[2].text = "Exit";
	buttons[2].textColor = WHITE;
	buttonNum = 3;
	while(true)
	{
		updateInput();
		paint();
		
		if(buttons[0].check()) return IS_SERVER;
		if(buttons[1].check()) return IN_JOIN_MENU;
		if(buttons[2].check()) return EXIT_GAME;
	}
	return EXIT_GAME;
}

int JOIN_MENU()
{
	BUTTON tempButton;
	tempButton.text = "";
	tempButton.focusColor = RED;
	tempButton.rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3}), POINT({windowW/2+200,windowH/3+40})});
	entryBoxes[0].center = true;
	entryBoxes[0]._button = tempButton;
	entryBoxes[0].textColor = WHITE;
	entryBoxes[0].tip = "Enter Server IP Address";
	entryBoxes[0].tipColor = RGB(200,200,200);
	buttons[0].center = true;
	buttons[0].focusColor = RGB(60,60,60);
	buttons[0].rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3+60}), POINT({windowW/2+200,windowH/3+100})});
	buttons[0].text = "Join";
	buttons[0].textColor = WHITE;
	buttons[1].center = true;
	buttons[1].focusColor = RGB(60,60,60);
	buttons[1].rect = myRECTANGLE({true, RGB(40,40,40), POINT({windowW/2-200,windowH/3+120}), POINT({windowW/2+200,windowH/3+160})});
	buttons[1].text = "Return";
	buttons[1].textColor = WHITE;
	entryBoxNum = 1;
	buttonNum = 2;
	while(true)
	{
		updateInput();
		paint();
		
		if(buttons[0].check()) return IS_CLIENT;
		if(buttons[1].check()) return IN_MAIN_MENU;
	}
	return EXIT_GAME;
}

void init()
{
	initgraph(1440,900,1);
//	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);
	windowW = getwidth();
	windowH = getheight();
	buttonNum = 0;
	entryBoxNum = 0;
	
//	createFood(1000);
	loadTexture("background.png",zoomFactor * 1500,zoomFactor * 1500);
	return;
}

void paint()
{
	BeginBatchDraw();
	cleardevice();
	paintTexture(-(int)floor(cameraX * zoomFactor)%(int)floor(60 * zoomFactor) - 120 * zoomFactor,-(int)floor(cameraY * zoomFactor)%(int)floor(60 * zoomFactor) - 120 * zoomFactor,"background.png");
//	paintTexture(-cameraX * zoomFactor,-cameraY * zoomFactor, "background.png");
//	paintTexture(-cameraX * zoomFactor,-cameraY * zoomFactor, "background.png");
	// Paint buttons
	for(register int i = 0; i < buttonNum; i++)
	{
		buttons[i].draw();
	}
	// Paint entry boxes
	for(register int i = 0; i < entryBoxNum; i++)
	{
		entryBoxes[i].draw();
	}
	if(startGame)
	{
		map<SOCKET, PLAYER>::iterator it;
		for(it = players.begin(); it != players.end(); it++)
		{
			it->second.draw();
		}
		for(register int i = 0; i < foods.size(); i++)
		{
			foods[i].draw();
		}
	}
	FlushBatchDraw();
	return;
}

void updateInput()
{
	_checkInput();
	register int i = 0;
	for(i = 0; i < entryBoxNum; i++)
	{
		entryBoxes[i].checkInput();
	}
	return;
}

void tickGame()
{
	
	if(startGame)
	{
		players[0].move();
		players[0].checkEat();
		players[0].update(windowW/2,windowH/2);
		if(players[0].checkDie())
		{
			Sleep(500);
			printf("DIE !!!\n");
			closegraph();
			system("pause");
			exit(0);
		}
		cameraX = players[0].getHeadPos().first;
		cameraY = players[0].getHeadPos().second;
		map<SOCKET, PLAYER>::iterator it;
		for(it = players.begin(); it != players.end(); it++)
		{
			it->second.draw();
		}
	}
	
	return;
}
