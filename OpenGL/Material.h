#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

class Material
{
public:
	Shader* shader;
	unsigned int diffuse;
	unsigned int specular;
	vec3 ambient;
	float shininess;

	Material(Shader* _shader, unsigned int _diffuse, unsigned int _specular, vec3 _ambient, float _shininess);
	~Material();
};

