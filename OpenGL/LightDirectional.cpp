#include "LightDirectional.h"

LightDirectional::LightDirectional(vec3 _position, vec3 _angles, vec3 _color) :
	position(_position),
	angles(_angles),
	color(_color)
{
	UpdateDirection();
}

LightDirectional::~LightDirectional()
{
}

void LightDirectional::UpdateDirection() {
	direction = vec3(0.0f, 0.0f, 1.0f);	// pointing to z (forward)
	direction = rotateZ(direction, angles.z);
	direction = rotateX(direction, angles.x);
	direction = rotateY(direction, angles.y);
	direction = -1.0f * direction;
	return;
}