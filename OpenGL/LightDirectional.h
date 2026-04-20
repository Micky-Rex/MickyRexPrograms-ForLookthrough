#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
using namespace glm;

class LightDirectional
{
public:
	LightDirectional(vec3 _position, vec3 _angles, vec3 _color = vec3(1.0f, 1.0f, 1.0f));
	~LightDirectional();

	vec3 position;
	vec3 angles;
	vec3 direction = vec3(0.0f, 0.0f, 1.0f);	// pointing to z (forward)
	vec3 color;

	void UpdateDirection();
};