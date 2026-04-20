#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Shader
{
public:
	string vertexString;
	string fragmentString;
	Shader(const char* vertexPath, const char* fragmentPath);
	const char* vertexSource;
	const char* fragmentSource;
	unsigned int ID;	//Shader Program ID

	enum Slot {
		DIFFUSE,
		SPECULAR
	};

	void use();
	void SetUniform3f(const char* paramNameString, vec3 param);
	void SetUniform1f(const char* paramNameString, float param);
	void SetUniform1i(const char* paramNameString, int slot);
	string SetLightUniformLocation(string light, unsigned int n, string value);

private:
	void checkCompileErrors(unsigned int ID, string type);
};
