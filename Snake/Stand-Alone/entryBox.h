
#include "Input.h"
#include "button.h"
#include <graphics.h>

#ifndef ENTRYBOX_H
#define ENTRYBOX_H

using namespace std;

struct ENTRY_BOX
{
	bool focus;
	bool center;
	string text,tip;
	COLORREF textColor,tipColor;
	BUTTON _button;
	string getText()
	{
		return text;
	}
	void checkInput()
	{
		if(_button.check()) focus = true;
		else
		{
			if(KEY_DOWN(VK_LBUTTON))
			{
				focus = false;
			}
		}
		if(focus)
		{
			register map<int,bool>::iterator it;
			for(it=inputMap.begin();it!=inputMap.end();it++)
			{
				if(it->second and !lastInputMap[it->first])
				{
					if(	it->first != VK_UP and
						it->first != VK_DOWN and
						it->first != VK_LEFT and
						it->first != VK_RIGHT and
						it->first != VK_BACK	)
					{
						if(it->first == VK_OEM_PERIOD)
						{
							text += '.';
						}
						else
						{
							text += it->first;
						}
					}
					else
					{
						if(it->first == VK_BACK)
						{
							if(text.size() > 0)
							{
								text.erase(text.size() - 1);
							}
							
						}
					}
					
				}
			}
			
		}
	}
	void draw()
	{
		if(text.empty())
		{
			_button.text = tip;
			_button.textColor = tipColor;
		}
		else
		{
			_button.text = text;
			_button.textColor = textColor;
		}
		
		_button.focus = focus;
		_button.draw();
		return;
	}
};
#endif
