#pragma once

#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"
#include "Camera.h"

namespace Brickview
{

	class CameraController
	{
	public:
		CameraController(const glm::vec3& target = {0.0f, 0.0f, 0.0f});

		const Camera& getCamera() const { return m_camera; }

		void setTargetPoint(const glm::vec3& targetPoint) { m_targetPoint = targetPoint; updatePosition(); }

		void onEvent(Event& e);

	private:
		void updatePosition();

		bool onWindowResize(const WindowResizeEvent& e);
		bool onMouseMoved(const MouseMovedEvent& e);
		bool onMousePressed(const MousePressedEvent& e);
		bool onMouseScrolled(const MouseScrolledEvent& e);

	private:
		Camera m_camera;

		glm::vec3 m_targetPoint = { 0.0f, 0.0f, 0.0f };

		glm::ivec2 m_currentMousePosition;
		glm::ivec2 m_mouseOffset = { 0.0f, 0.0f };

		float m_angleSensitivity = 0.15f;
		float m_translationSensitivity = m_angleSensitivity / 50.0f;
		float m_distanceFromObject = 2.0f;
	};

}