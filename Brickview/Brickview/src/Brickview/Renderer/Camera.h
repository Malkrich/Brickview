#pragma once

#include "Metric/World.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class Camera
	{
	public:
		Camera(const glm::vec2& dimensions, const glm::vec3 & position = glm::vec3(0.0f), float pitch = 0.0f, float yaw = 0.0f);

		void setViewportDimension(uint32_t width, uint32_t height);

		const glm::vec3& getPosition() const { return m_position; }
		void setPosition(const glm::vec3& position) { m_position = position; updateViewMatrix(); }

		float getPitch() const { return m_pitch; }
		float getYaw() const { return m_yaw; }
		glm::vec3 getUpVector() const { return m_cameraVectorTransform * World::getUpVector(); }
		glm::vec3 getRightVector() const { return m_cameraVectorTransform * World::getXUnitVector(); }
		void setRotation(float pitch, float yaw)
		{
			m_pitch = pitch;
			m_yaw = yaw;
			updateViewMatrix();
		}

		glm::mat4 getViewProjectionMatrix() const { return m_projectionMatrix * m_viewMatrix; }

	private:
		void updateViewMatrix();
		void updateProjectionMatrix();

	private:
		float m_fov = 45.0f, m_nearClip = 0.01f, m_farClip = 100.0f;
		float m_viewportWidth , m_viewportHeight;
		float m_aspectRatio;

		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;

		glm::vec3 m_position;
		// https://learnopengl.com/Getting-Started/Camera
		// In degrees
		float m_pitch, m_yaw;
		glm::mat3 m_cameraVectorTransform;
	};

}