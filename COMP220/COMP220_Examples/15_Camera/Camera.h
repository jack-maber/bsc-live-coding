#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Camera
{
public:
	Camera();

	void FPSUpdate();
	void Update();

private:
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraTarget;
	glm::vec3 m_CameraUp;
	glm::vec3 m_CameraDirection;
	glm::vec3 m_FPScameraPos;
	float m_CameraX;
	float m_CameraY;
	float m_CameraDistance;

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;

};