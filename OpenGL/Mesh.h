#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <assimp/types.h>

using namespace std;
using namespace glm;

struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	Mesh(float vertices[]);
	Mesh(	vector<Vertex> vertices,
			vector<unsigned int> indices,
			vector<Texture> textures);
	~Mesh();
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	void Draw(Shader* shader);
private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

