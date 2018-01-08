#include "Camera.h"

Camera::Camera()
{
	
	// Camera Properties
	m_CameraPosition = glm::vec3(0.0f, 8.0f, -30.0f);
	m_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_CameraDirection = glm::vec3(0.0f);
	m_FPScameraPos = glm::vec3(0.0f);
	m_CameraX = 0.0f;
	m_CameraY = 0.0f;
	m_CameraDistance = (float)(m_CameraTarget - m_CameraPosition).length();

	
	m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraTarget, m_CameraUp);
	m_ProjectionMatrix = glm::perspective(glm::radians(90.0f), float(800 / 600), 0.1f, 100.0f);

}

//Update Function for the FPS Camera 
void Camera::FPSUpdate()
{
	m_CameraPosition += m_FPScameraPos;
	m_CameraTarget += m_FPScameraPos;
}
//Updates the View Matrix
void Camera::ViewUpdate()
{
	m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraTarget, m_CameraUp);
}


