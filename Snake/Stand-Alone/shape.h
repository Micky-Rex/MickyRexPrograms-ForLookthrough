
#include <math.h>
#ifndef SHAPE_H
#define SHAPE_H
using namespace std;

struct myRECTANGLE
{
	bool isFull;
	COLORREF color;
	POINT tl,dr;
};

struct myCIRCLE
{
	bool isFull;
	COLORREF color;
	POINT pos;
	int radians;
};
#endif
