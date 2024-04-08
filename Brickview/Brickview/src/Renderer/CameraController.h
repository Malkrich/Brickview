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

		void onEvent(Event& e);

	private:
		void updatePosition();

		bool onWindowResize(const WindowResizeEvent& e);
		bool onMouseMove(const MouseMoveEvent& e);
		bool onMousePressed(const MousePressedEvent& e);
		bool onMouseScrolled(const MouseScrolledEvent& e);

	private:
		Camera m_camera;

		// TODO : Implement target point.
		glm::ivec2 m_currentMousePosition;
		glm::ivec2 m_mouseOffset = { 0.0f, 0.0f };

		float m_angleSensitivity = 0.15f;
		float m_distanceFromObject = 2.0f;
	};

}