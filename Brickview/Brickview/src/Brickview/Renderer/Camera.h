#pragma once

#include "Metric/World.h"

#include <glm/glm.hpp>

namespace Brickview
{

	class Camera
	{
	public:
		Camera(const glm::vec3 & position, float pitch, float yaw);

		virtual glm::mat4 getViewProjectionMatrix() const = 0;
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }

		// Camera position
		const glm::vec3& getPosition() const { return m_position; }
		void setPosition(const glm::vec3& position) { m_position = position; updateViewMatrix(); }

		// Camera orientation
		float getPitch() const { return m_pitch; }
		float getYaw() const { return m_yaw; }
		void setRotation(float pitch, float yaw)
		{
			m_pitch = pitch;
			m_yaw = yaw;
			updateViewMatrix();
		}
		glm::vec3 getUpVector() const { return m_cameraVectorTransform * World::getUpVector(); }
		glm::vec3 getRightVector() const { return m_cameraVectorTransform * World::getXUnitVector(); }

	private:
		void updateViewMatrix();

	private:
		glm::mat4 m_viewMatrix;

		glm::vec3 m_position;
		// https://learnopengl.com/Getting-Started/Camera
		// In degrees
		float m_pitch, m_yaw;
		glm::mat3 m_cameraVectorTransform;
	};

}