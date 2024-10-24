#include "Pch.h"
#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Brickview
{

	Camera::Camera(const glm::vec3& position, float pitch, float yaw)
		: m_position(position)
		, m_pitch(pitch)
		, m_yaw(yaw)
	{
		updateViewMatrix();
	}

	void Camera::updateViewMatrix()
	{
		glm::quat orientation = glm::quat(glm::vec3(glm::radians(m_pitch), glm::radians(m_yaw), 0.0f));

		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_cameraVectorTransform = glm::mat3(m_viewMatrix);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

}