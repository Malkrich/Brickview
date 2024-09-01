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

	void Camera::setViewportDimension(uint32_t width, uint32_t height)
	{
		m_viewportWidth = (float)width;
		m_viewportHeight = (float)height;
		updateProjectionMatrix();
	}

	void Camera::updateViewMatrix()
	{
		glm::quat orientation = glm::quat(glm::vec3(glm::radians(m_pitch), glm::radians(m_yaw), 0.0f));

		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	void Camera::updateProjectionMatrix()
	{
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
	}

}