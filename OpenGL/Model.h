#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "shader.h"
#include "stb_image.h"

using namespace std;
using namespace glm;
using namespace Assimp;

class Model
{
public:
	Model(string path);
	~Model();
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;
	void Draw(Shader* shader);

private:
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	unsigned int loadTexture(const string path);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void processTexture(aiMaterial* material, vector<Texture>& vec);
};