#include "Pch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Brickview
{

	PerspectiveCamera::PerspectiveCamera(float width, float height, const glm::vec3& position, float pitch, float yaw, float fov, float nearClip, float farClip)
		: Camera(position, pitch, yaw)
		, m_viewportWidth(width)
		, m_viewportHeight(height)
		, m_aspectRatio(m_viewportWidth / m_viewportHeight)
		, m_fov(fov)
		, m_nearClip(nearClip)
		, m_farClip(farClip)
	{
		updateProjectionMatrix();
	}

	void PerspectiveCamera::setViewportDimensions(float width, float height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		updateProjectionMatrix();
	}

	void PerspectiveCamera::updateProjectionMatrix()
	{
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
	}

}