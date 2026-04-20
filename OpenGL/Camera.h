#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class Camera
{
public:
	Camera(vec3 position, vec3 target, vec3 worldup);
	Camera(vec3 position, float pitch, float yaw, vec3 worldup);

	vec3 Position;
	vec3 Forward;
	vec3 Right;
	vec3 Up;
	vec3 WorldUp;
	float speed = 0.02f;
	float Pitch;
	float Yaw;
	float SenseX = -0.001f;
	float SenseY = -0.001f;
	float speedX = 0;
	float speedY = 0;
	float speedZ = 0;
	mat4 GetViewMatrix();
	void ProcessMouseMovement(float deltaX, float deltaY);
	void UpdateCameraPos();
	//void SetSpeedZ(float speed);

private:
	void UpdateCameraVectors();
};

