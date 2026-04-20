
#include "AABB.h"
#include "Input.h"
#include "shape.h"
#include <windows.h>
#include <graphics.h>

using namespace std;

#ifndef BUTTON_H
#define BUTTON_H
struct BUTTON
{
	bool focus;
	bool center;
	string text;
	COLORREF textColor;
	COLORREF focusColor;
	myRECTANGLE rect;
	bool check()
	{
		if(KEY_DOWN(VK_LBUTTON) and isContact(rect, mouse))
		{
			return true;
		}
		return false;
	}
	void draw()
	{	
		if(focus)
		{
			setcolor(focusColor);
			setfillcolor(focusColor);
		}
		else
		{
			setcolor(rect.color);
			setfillcolor(rect.color);
		}
		fillrectangle(rect.tl.x, rect.tl.y, rect.dr.x, rect.dr.y);
		setcolor(rect.color);
		setfillcolor(rect.color);
		fillrectangle(rect.tl.x+4, rect.tl.y+4, rect.dr.x-4, rect.dr.y-4);
		settextcolor(textColor);
		settextstyle(30,0,"·ÂËÎ");
		
		int tx,ty;
		tx = rect.tl.x + (rect.dr.x - rect.tl.x) / 2;
		ty = rect.tl.y + (rect.dr.y - rect.tl.y) / 2;
		if(center)
		{
			tx -= textwidth(text.c_str()) / 2;
			ty -= textheight(text.c_str()) / 2;
		}
		outtextxy(tx,ty,text.c_str());
		return;
	}
};
#endif
