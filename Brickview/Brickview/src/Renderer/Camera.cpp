#include "Pch.h"
#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Brickview
{

	Camera::Camera()
		: m_projectionMatrix(glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip))
	{
		updateViewMatrix();
	}

	void Camera::setViewportDimension(unsigned int width, unsigned int height)
	{
		m_viewportWidth = (float)width;
		m_viewportHeight = (float)height;
		updateProjectionMatrix();
	}

	void Camera::updateViewMatrix()
	{
		//m_viewMatrix = glm::lookAt(m_position, m_position + m_lookAt, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat orientation = glm::angleAxis(glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))
			* glm::angleAxis(glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	void Camera::updateProjectionMatrix()
	{
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
	}

}