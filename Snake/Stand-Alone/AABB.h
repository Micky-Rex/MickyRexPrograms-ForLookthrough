
#include "shape.h"
#include "myMath.h"
#ifndef AABB_H
#define AABB_H
using namespace std;

bool isContact(myRECTANGLE shape1, myRECTANGLE shape2)
{
	POINT p1[4],p2[4];
	p1[0] = shape1.tl;
	p1[1] = POINT({shape1.dr.x,shape1.tl.y});
	p1[2] = shape1.dr;
	p1[3] = POINT({shape1.tl.x,shape1.dr.y});
	
	p2[0] = shape2.tl;
	p2[1] = POINT({shape2.dr.x,shape2.tl.y});
	p2[2] = shape2.dr;
	p2[3] = POINT({shape2.tl.x,shape2.dr.y});
	
	// shape1右下角或左上角 进入 shape2 
	if(	p1[2].x >= p2[0].x
		and p1[2].y >= p2[0].y
		and p1[0].x <= p2[2].x
		and p1[0].y <= p2[2].y)
	{
		return true;
	}
	
	// shape1右上角或左下角 进入 shape2 
	if(	p1[1].x <= p2[3].x
		and p1[1].y <= p2[3].y
		and p1[3].x >= p2[1].x
		and p1[3].y >= p2[1].y)
	{
		return true;
	}
	return false;
}

bool isContact(myRECTANGLE shape1, POINT shape2)
{
	POINT p1[4];
	p1[0] = shape1.tl;
	p1[1] = POINT({shape1.dr.x,shape1.tl.y});
	p1[2] = shape1.dr;
	p1[3] = POINT({shape1.tl.x,shape1.dr.y});
	
	if(	p1[2].x >= shape2.x
		and p1[2].y >= shape2.y
		and p1[0].x <= shape2.x
		and p1[0].y <= shape2.y)
	{
		return true;
	}
	
	if(	p1[1].x <= shape2.x
		and p1[1].y <= shape2.y
		and p1[3].x >= shape2.x
		and p1[3].y >= shape2.y)
	{
		return true;
	}
	return false;
}

bool isContact(myCIRCLE p1, myCIRCLE p2)
{
	if(__distance(p1.pos, p2.pos) <= p1.radians + p2.radians)
	{
		return true;
	}
	return false;
}

bool isContact(myCIRCLE p1, POINT p2)
{
	if(__distance(p1.pos, p2) <= p1.radians)
	{
		return true;
	}
	return false;
}
#endif
