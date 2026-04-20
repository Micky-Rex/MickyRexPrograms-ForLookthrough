#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;


Camera::Camera(vec3 position, vec3 target, vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Forward = normalize(target - position);
	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Forward, Right));
}

Camera::Camera(vec3 position, float pitch, float yaw, vec3 worldup)
{
	Position = position;
	WorldUp = worldup;
	Pitch = pitch;
	Yaw = yaw;
	Forward.x = cos(pitch) * sin(yaw);
	Forward.y = sin(pitch);
	Forward.z = cos(pitch) * cos(yaw);
	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Forward, Right));
}


mat4 Camera::GetViewMatrix()
{
	return lookAt(Position, Position + Forward, WorldUp);
}

void Camera::UpdateCameraVectors()
{
	Forward.x = cos(Pitch) * sin(Yaw);
	Forward.y = sin(Pitch);
	Forward.z = cos(Pitch) * cos(Yaw);
	Right = normalize(cross(Forward, WorldUp));
	Up = normalize(cross(Right, Forward));
}

void Camera::ProcessMouseMovement(float deltaX, float deltaY)
{
	Pitch += deltaY * SenseY;
	Yaw += deltaX * SenseX;
	if (Pitch >= 1.56) Pitch = 1.55999f;
	if (Pitch <= -1.56) Pitch = -1.55999f;
	UpdateCameraVectors();
}

void Camera::UpdateCameraPos()
{
	Position += normalize(cross(Forward, WorldUp)) * speedX * -speed;
	Position += normalize(cross(Forward, Right)) * speedY * -speed;
	Position += Forward * speedZ * speed;
}
