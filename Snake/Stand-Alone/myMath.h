
#ifndef MYMATH_H
#define MYMATH_H
#include <cmath>
using namespace std;
struct ivec2
{
	int x,y;
};
struct ivec3
{
	int x,y,z;
};
struct ivec4
{
	int x,y,z,w;
};
struct vec2
{
	float x,y;
};
struct vec3
{
	float x,y,z;
};
struct vec4
{
	float x,y,z,w;
};

float _radToDeg(float radians)
{
	return radians * (180.0 / M_PI);
}

float _degToRad(float degrees)
{
	return degrees / (180.0 / M_PI);
}

float angleToXAxis(int x1,int y1,int x2,int y2)
{
	float angleRad = atan2(x2 - x1, y2 - y1);
	float angleDeg = angleRad - M_PI / 2;
	if(angleDeg < 0) angleDeg += M_PI * 2;
	if(angleDeg > M_PI * 2) angleDeg -= M_PI * 2;
	return angleDeg;
}
float __distance(POINT p1, POINT p2)
{
	int x1,x2,y1,y2;
	x1 = p1.x, y1 = p1.y;
	x2 = p2.x, y2 = p2.y;
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
float __distance(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
int clamp(int x, int maxx, int minx)
{
	if(x > maxx) x = maxx;
	if(x < minx) x = minx;
	return x;
}
float clampf(float x, float maxx, float minx)
{
	if(x > maxx) x = maxx;
	if(x < minx) x = minx;
	return x;
}
#endif
