#pragma once

#include "Camera.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float width, float height, const glm::vec3& position, float pitch, float yaw, float fov = 45.0f, float nearClip = 0.01f, float farClip = 100.0f);

		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		virtual glm::mat4 getViewProjectionMatrix() const override { return m_projectionMatrix * getViewMatrix(); }

		void setViewportDimensions(float width, float height);

	private:
		void updateProjectionMatrix();

	private:
		// Projection matrix parameters
		float m_viewportWidth, m_viewportHeight, m_aspectRatio;
		float m_fov, m_nearClip, m_farClip;

		glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
	};

}