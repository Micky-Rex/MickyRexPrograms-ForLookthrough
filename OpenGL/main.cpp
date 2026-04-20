#pragma warning (disable : 4244)
#pragma warning (disable : 26495)
#include <iostream>
#define GLEW_STATIC
//glfw, glew
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "LightDirectional.h"
#include "LightPoint.h"
#include "LightSpot.h"
#include "Mesh.h"
#include "Model.h"

//stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NR_DIRECTIONAL_LIGHTS 1
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 1

using namespace std;
using namespace glm;

#pragma region Model Data
float vertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	// Front face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	// Left face
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	// Right face
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
	 // Bottom face
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	 // Top face
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
};
//	顶点绘制顺序
unsigned int indices[] = {
	0,1,2,
	2,3,0
};
vec3 cubePositions[] = {
	vec3(0.0f,  0.0f,  0.0f),
	vec3(2.0f,  5.0f, -15.0f),
	vec3(-1.5f, -2.2f, -2.5f),
	vec3(-3.8f, -2.0f, -12.3f),
	vec3(2.4f, -0.4f, -3.5f),
	vec3(-1.7f,  3.0f, -7.5f),
	vec3(1.3f, -2.0f, -2.5f),
	vec3(1.5f,  2.0f, -2.5f),
	vec3(1.5f,  0.2f, -1.5f),
	vec3(-12.3f,  1.0f, -1.5f)
};
#pragma endregion
#pragma region Camera Declare
//Instantiate Camera class
//Camera camera(vec3(0, 0, 3.0f), vec3(0, 0, 0), vec3(0, 1.0f, 0));
Camera camera(vec3(0, 0, 3.0f), radians(-15.0f), radians(180.0f), vec3(0, 1.0f, 0));
#pragma endregion
#pragma region Light Declare
LightDirectional lightD[NR_DIRECTIONAL_LIGHTS] = {
	LightDirectional(vec3(0.0f, 0.0f, 0.0f), vec3(radians(45.0f), radians(0.0f), radians(0.0f)), vec3(5.0f, 5.0f, 5.0f))
};
LightPoint lightP[NR_POINT_LIGHTS] = {
	LightPoint(vec3(0.05f, 4.0f, 0.0f), vec3(0,0,0), vec3(1.0f, 0.0f, 0.0f)),
	LightPoint(vec3(0.0f, 0.0f, 3.0f), vec3(0,0,0), vec3(0.0f, 1.0f, 0.0f)),
	LightPoint(vec3(0.05f, -4.0f, 0.0f), vec3(0,0,0), vec3(0.0f, 0.0f, 1.0f)),
	LightPoint(vec3(10.0f, 10.0f, 10.0f), vec3(0,0,0), vec3(0.0f, 0.0f, 0.0f))
};
LightSpot lightS[NR_SPOT_LIGHTS] = {
	LightSpot(vec3(0.0f, 5.0f, 0.0f), vec3(radians(90.0f),radians(0.0f),radians(0.0f)), vec3(1.0f, 1.0f, 1.0f))
};
#pragma endregion
#pragma region Input Declare

	float lastX;
	float lastY;
	bool firstMouse =true;

	void processinput(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.speedX = 1.0f;
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.speedX = -1.0f;
		}
		else
		{
			camera.speedX = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera.speedY = 1.0f;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		{
			camera.speedY = -1.0f;
		}
		else
		{
			camera.speedY = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.speedZ = 1.0f;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.speedZ = -1.0f;
		}
		else
		{
			camera.speedZ = 0;
		}
	}

	void mouse_callback(GLFWwindow* window, double xPos, double yPos)
	{
		if (firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}
		float deltaX, deltaY;
		deltaX = xPos - lastX;
		deltaY = yPos - lastY;
		lastX = xPos;
		lastY = yPos;
		camera.ProcessMouseMovement(deltaX, deltaY);
		//printf("%f %f\n", deltaX, deltaY);
	}

#pragma endregion

//unsigned int LoadImageToGPU(const char* filename, GLint internalFormat, GLenum format,int textureSlot) {
//	unsigned int TexBuffer;
//	glGenTextures(1, &TexBuffer);
//	glActiveTexture(GL_TEXTURE0 + textureSlot);
//	glBindTexture(GL_TEXTURE_2D, TexBuffer);
//	int width, height, nrChannel;
//	stbi_set_flip_vertically_on_load(true);
//	unsigned char* data = stbi_load(filename, &width, &height, &nrChannel, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		printf("load image failed.\n");
//	}
//	stbi_image_free(data);
//	return TexBuffer;
//}


int main(int argc, char* argv[]) {
	string exePath = argv[0];

	#pragma region Open a Window
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//Open GLFW Window
		GLFWwindow* window = glfwCreateWindow(800, 600, "My OpenGL", NULL, NULL);

		if (window == NULL)
		{
			printf("Open Window failed.");
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);

		// Init GLFW
		glewExperimental = true;
		if (glewInit() != GLEW_OK)
		{
			printf("Init GLEW failed.");
			glfwTerminate();
			return -1;
		}
		glGetError();	// clear error flag
		glViewport(0, 0, 800, 600);
	#pragma endregion
	//开启顶点深度测试
	glEnable(GL_DEPTH_TEST);
	#pragma region Init Shader Program
		Shader* myShader = new Shader("vertexSource.vs", "fragmentSource.fs");
	#pragma endregion
	/*
	#pragma region Init Material
		Material* myMaterial =
			new Material(
				myShader,
				LoadImageToGPU("container2.png", GL_RGBA, GL_RGBA, Shader::DIFFUSE),
				LoadImageToGPU("container2_specular.png", GL_RGBA, GL_RGBA, Shader::SPECULAR),
				vec3(1.0f, 1.0f, 1.0f), 64.0f);

	#pragma endregion
	*/
	
	//Material* myMaterial =
	//	new Material(
	//		myShader,
	//		LoadImageToGPU("container2.png", GL_RGBA, GL_RGBA, Shader::DIFFUSE),
	//		LoadImageToGPU("container2_specular.png", GL_RGBA, GL_RGBA, Shader::SPECULAR),
	//		vec3(1.0f, 1.0f, 1.0f), 64.0f);
	Material* myMaterial = new Material(myShader, 0, 0, vec3(1.0f, 1.0f, 1.0f), 64.0f);
	
	Model model(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\nanosuit.obj");

	#pragma region Prepare MVP matrices
		mat4 modelMat;
		mat4 viewMat;
		mat4 projMat;
		projMat = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	#pragma endregion

	while (!glfwWindowShouldClose(window))
	{
		// Process Input
		processinput(window);

		// Clear Screen
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//顶点坐标深度测试

		//
		viewMat = camera.GetViewMatrix();

		for (int i = 0; i < 1; i++) {
			// Set Model matrix
			modelMat = translate(mat4(1.0f), cubePositions[i]);
			// Set View and Projection Matrices here if you want
			
			// Set Material -> Shader Program
			myShader->use();
			glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "modelMat"), 1, GL_FALSE, value_ptr(modelMat));
			glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "viewMat"), 1, GL_FALSE, value_ptr(viewMat));
			glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "projMat"), 1, GL_FALSE, value_ptr(projMat));
			glUniform3f(glGetUniformLocation(myShader->ID, "objColor"), 1.0f, 1.0f, 1.0f);
			glUniform3f(glGetUniformLocation(myShader->ID, "ambientColor"), 0.2f, 0.2f, 0.2f);
			#pragma region Set lights 
				// Lights
				myShader->SetUniform1i("NR_DIRECTIONAL_LIGHTS", NR_DIRECTIONAL_LIGHTS);
				for (int j = 0; j < NR_DIRECTIONAL_LIGHTS; j++) {
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightD[", j, "].pos").c_str(), lightD[j].position);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightD[", j, "].dirToLight").c_str(), lightD[j].direction);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightD[", j, "].color").c_str(), lightD[j].color);
				}

				myShader->SetUniform1i("NR_POINT_LIGHTS", NR_POINT_LIGHTS);
				for (int j = 0; j < NR_POINT_LIGHTS; j++) {
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightP[", j, "].pos").c_str(), lightP[j].position);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightP[", j, "].dirToLight").c_str(), lightP[j].direction);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightP[", j, "].color").c_str(), lightP[j].color);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightP[", j, "].constant").c_str(), lightP[j].constant);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightP[", j, "].linear").c_str(), lightP[j].linear);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightP[", j, "].quadratic").c_str(), lightP[j].quadratic);
				}

				myShader->SetUniform1i("NR_SPOT_LIGHTS", NR_SPOT_LIGHTS);
				for (int j = 0; j < NR_SPOT_LIGHTS; j++) {
					//myShader->SetUniform3f(myShader->SetLightUniformLocation("lightS[", j, "].pos").c_str(), lightS[j].position);
					//myShader->SetUniform3f(myShader->SetLightUniformLocation("lightS[", j, "].dirToLight").c_str(), lightS[j].direction);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightS[", j, "].pos").c_str(), camera.Position);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightS[", j, "].dirToLight").c_str(), -camera.Forward);
					myShader->SetUniform3f(myShader->SetLightUniformLocation("lightS[", j, "].color").c_str(), lightS[j].color);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightS[", j, "].constant").c_str(), lightP[j].constant);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightS[", j, "].linear").c_str(), lightP[j].linear);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightS[", j, "].quadratic").c_str(), lightP[j].quadratic);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightS[", j, "].cosPhyInner").c_str(), lightS[j].cosPhyInner);
					myShader->SetUniform1f(myShader->SetLightUniformLocation("lightS[", j, "].cosPhyOuter").c_str(), lightS[j].cosPhyOuter);
				}
			#pragma endregion
			// Camera
			myShader->SetUniform3f("cameraPos", camera.Position);
			// Material
			myMaterial->shader->SetUniform3f("material.ambient", myMaterial->ambient);
			myMaterial->shader->SetUniform1f("material.shininess", myMaterial->shininess);
			//myMaterial->shader->SetUniform1i("material.diffuse", Shader::DIFFUSE);
			//myMaterial->shader->SetUniform1i("material.specular", Shader::SPECULAR);

			//model.Draw(myShader);
			model.Draw(myMaterial->shader);
			//model.meshes[2].Draw(myMaterial->shader);
			
		}
		
		// Clean up, propare for next render loop
		glfwSwapBuffers(window);
		glfwPollEvents();
		camera.UpdateCameraPos();
	}
	// Exit program
	glfwTerminate();
	return 0;

}