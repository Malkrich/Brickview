#pragma once

namespace Brickview
{

	class Camera
	{
	public:
		Camera();

		void setViewportDimension(unsigned int width, unsigned int height);

		const glm::vec3& getPosition() const { return m_position; }
		void setPosition(const glm::vec3& position) { m_position = position; updateViewMatrix(); }

		float getPitch() const { return m_pitch; }
		float getYaw() const { return m_yaw; }
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
		float m_fov = 45.0f, m_nearClip = 0.1f, m_farClip = 1000.0f;
		float m_viewportWidth = 1280.0f, m_viewportHeight = 720.0f;
		float m_aspectRatio = m_viewportWidth / m_viewportHeight;

		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;

		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		// https://learnopengl.com/Getting-Started/Camera
		float m_pitch = 0.0f, m_yaw = 0.0f;
	};

}