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

		void setTargetPoint(const glm::vec3& targetPoint) { m_targetPoint = targetPoint; }

		void onEvent(Event& e);

	private:
		bool onWindowResize(const WindowResizeEvent& e);
		bool onMouseMove(const MouseMoveEvent& e);
		bool onMousePressed(const MousePressedEvent& e);

	private:
		Camera m_camera;

		glm::vec3 m_targetPoint;
		glm::ivec2 m_currentMousePosition;
	};

}