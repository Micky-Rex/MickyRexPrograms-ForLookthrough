#include "LightPoint.h"

LightPoint::LightPoint(vec3 _position, vec3 _angles, vec3 _color) :
	position(_position),
	angles(_angles),
	color(_color)
{
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
	return;
}

LightPoint::~LightPoint()
{
	return;
}
