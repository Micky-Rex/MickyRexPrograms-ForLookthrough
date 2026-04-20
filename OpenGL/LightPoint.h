#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
using namespace glm;

class LightPoint
{
public:
	LightPoint(vec3 _position, vec3 _angles, vec3 _color = vec3(1.0f, 1.0f, 1.0f));
	~LightPoint();

	vec3 position, angles, direction = vec3(0.0f, 0.0f, 1.0f), color;
	float constant, linear, quadratic;
};