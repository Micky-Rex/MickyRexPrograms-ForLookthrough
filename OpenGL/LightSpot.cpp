#include "LightSpot.h"

LightSpot::LightSpot(vec3 _position, vec3 _angles, vec3 _color) :
	position(_position),
	angles(_angles),
	color(_color)
{
	constant = 1.0f;
	linear = 0.9f;
	quadratic = 0.32f;
	UpdateDirection();
	return;
}

LightSpot::~LightSpot()
{
	return;
}

void LightSpot::UpdateDirection()
{
	direction = vec3(0.0f, 0.0f, 1.0f);	// pointing to z (forward)
	direction = rotateZ(direction, angles.z);
	direction = rotateX(direction, angles.x);
	direction = rotateY(direction, angles.y);
	direction = -1.0f * direction;
	return;
}
