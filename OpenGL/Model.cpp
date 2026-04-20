#include "Model.h"

Model::Model(string path)
{
	loadModel(path);
	return;
}
Model::~Model()
{
	return;
}
void Model::Draw(Shader* shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
	return;
}
void Model::loadModel(string path)
{
	Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or !scene->mRootNode)
	{
		printf("Assimp : ERROR\n");
		return;
	}
	printf("Assimp : Success!\n");
	directory = path.substr(0, path.find_last_of('\\'));
	//printf("%s\n", directory.c_str());
	processNode(scene->mRootNode, scene);
	return;
}
void Model::processNode(aiNode* node, const aiScene* scene)
{
	printf("Assimp : Load Node %s\n", node->mName.data);
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* curMesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(curMesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
	return;
}
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	Vertex tempVertex;
	vector<Vertex> tempVertices;
	vector<unsigned int> tempIndices;
	vector<Texture> tempTextures;
	// Load Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		tempVertex.Position.x = mesh->mVertices[i].x;
		tempVertex.Position.y = mesh->mVertices[i].y;
		tempVertex.Position.z = mesh->mVertices[i].z;
		tempVertex.Normal.x = mesh->mNormals[i].x;
		tempVertex.Normal.y = mesh->mNormals[i].y;
		tempVertex.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTextureCoords[0])
		{
			tempVertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			tempVertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			tempVertex.TexCoords = vec2(0.0f, 0.0f);
		}
		tempVertices.push_back(tempVertex);
	}
	// Load Faces
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			tempIndices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	// Load Textures
	tempTextures.clear();
	if(mesh->mMaterialIndex >= 0)
	{
		processTexture(scene->mMaterials[mesh->mMaterialIndex], tempTextures);
	}

	return Mesh(tempVertices, tempIndices, tempTextures);
}
void Model::processTexture(aiMaterial* material, vector<Texture>& vec)
{
	Texture tex;
	aiString path;
	bool skip_load = false;

	// Texture(Diffuse)
	for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
		skip_load = false;
		material->GetTexture(aiTextureType_DIFFUSE, i, &path);
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (strcmp(textures_loaded[j].path.c_str(), path.C_Str()) == 0) {
				vec.push_back(textures_loaded[j]);
				skip_load = true;
				break;
			}
		}
		if (!skip_load) {
			tex.id = loadTexture(path.C_Str());
			tex.type = "texture_diffuse";
			tex.path = path.C_Str();
			vec.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}

	// Texture(Specular)
	for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++) {
		skip_load = false;
		material->GetTexture(aiTextureType_SPECULAR, i, &path);
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (strcmp(textures_loaded[j].path.c_str(), path.C_Str()) == 0) {
				vec.push_back(textures_loaded[j]);
				skip_load = true;
				break;
			}
		}
		if (!skip_load) {
			tex.id = loadTexture(path.C_Str());
			tex.type = "texture_specular";
			tex.path = path.C_Str();
			vec.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}

	return;
}
unsigned int Model::loadTexture(const string _path)
{
	unsigned int TexID;
	glGenTextures(1, &TexID);

	string path = this->directory + '\\' + _path;

	int width, height, nrChannel;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannel, 0);
	if (data)
	{
		GLenum format{};
		if (nrChannel == 1)
			format = GL_RED;
		else if (nrChannel == 3)
			format = GL_RGB;
		else if (nrChannel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, TexID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		printf("Assimp : load texture failed : %s\n", path.c_str());
	}
	printf("Assimp : %s\n", path.c_str());
	stbi_image_free(data);
	return TexID;
}