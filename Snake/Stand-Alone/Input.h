
#include <windows.h>

#ifndef INPUT_H
#define INPUT_H
#define KEY_DOWN(VK_NINAME)((GetAsyncKeyState(VK_NINAME)&0x8000)?1:0)

using namespace std;

POINT mouse;
map<int, bool> inputMap;
map<int, bool> lastInputMap;

void _checkKeyButton(int _c)
{
	if(_c >= 'a' and _c <= 'z')
	{
		lastInputMap[_c] = inputMap[_c];
		inputMap[_c] = KEY_DOWN(_c - 'a' + 'A');
	}
	else
	{
		lastInputMap[_c] = inputMap[_c];
		inputMap[_c] = KEY_DOWN(_c);
	}
	return;
}

void _checkInput()
{
	// KEY_BOARD
	register int i;
	for(i = 'A'; i <= 'Z'; i++)
	{
		if(KEY_DOWN(VK_SHIFT))
		{
			_checkKeyButton(i);
		}
		else
		{
			_checkKeyButton(i + 'a' - 'A');
		}
	}
	for(i = '0'; i <= '9'; i++)
	{
		_checkKeyButton(i);
	}
	_checkKeyButton(VK_UP);
	_checkKeyButton(VK_DOWN);
	_checkKeyButton(VK_LEFT);
	_checkKeyButton(VK_RIGHT);
	_checkKeyButton(VK_BACK);
	_checkKeyButton(VK_OEM_PERIOD);
	
	// MOUSE
	GetCursorPos(&mouse);
	ScreenToClient(GetForegroundWindow(),&mouse);
	return;
}
#endif
