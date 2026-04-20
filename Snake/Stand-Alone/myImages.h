#include <map>
#include <string>
#include <graphics.h>

#include "snake.h"
#ifndef MYIMAGES_H
#define MYIMAGES_H
using namespace std;

IMAGE images[MAX_TEXTURES + 5];
map<string,int> imageID;
void loadTexture(string _path, int w, int h)
{
	int cur = imageID.size();
	if(cur >= MAX_TEXTURES)
	{
		printf("[myImages] : load %s. Textures number exceeds MAX_TEXTURES_NUMBER(%d)!\n",_path.c_str(),MAX_TEXTURES);
	}
	imageID[_path] = cur;
	loadimage(&images[cur], string("./assets/textures/" + _path).c_str(), w, h);
	printf("[myImages] : %s as id %d.\n",_path.c_str(),cur);
	return;
}
int paintTexture(int _x,int _y,string _path)
{
	if(imageID.count(_path) == 0)
	{
		printf("[myImages - Error] : Asset %s not found!\n", _path.c_str());
		return -1;
	}
	putimage(_x,_y,&images[imageID[_path]]);
	return 0;
}
#endif
