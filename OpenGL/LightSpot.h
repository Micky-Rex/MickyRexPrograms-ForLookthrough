#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"

using namespace std;
using namespace glm;

class LightSpot
{
public:
	LightSpot(vec3 _position, vec3 _angles, vec3 _color = vec3(1.0f, 1.0f, 1.0f));
	~LightSpot();

	void UpdateDirection();

	vec3 position, direction = vec3(0.0f, 0.0f, 1.0f), angles, color;
	float cosPhyInner = 0.95f;
	float cosPhyOuter = 0.92f;
	float constant, linear, quadratic;
};