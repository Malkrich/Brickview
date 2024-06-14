#pragma once

#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/KeyCodes.h"
#include "Camera.h"

namespace Brickview
{

	struct CameraControllerSpecifications
	{
		glm::vec3 TargetPosition = { 0.0f, 0.0f, 0.0f };
		bool LaptopMode = false;
	};

	class CameraController
	{
	public:
		CameraController() = default;
		CameraController(const CameraControllerSpecifications& spec);

		const Camera& getCamera() const { return m_camera; }

		void resize(uint32_t width, uint32_t height) { m_camera.setViewportDimension(width, height); }
		void setTargetPoint(const glm::vec3& targetPoint) { m_targetPoint = targetPoint; updatePosition(); }
		void setViewportHovered(bool hovered) { m_isViewportHovered = hovered; }
		void setLaptopMode(bool laptopMode) { m_laptopMode = laptopMode; }

		void onEvent(Event& e);

	private:
		void updatePosition();

		bool onMouseMoved(const MouseMovedEvent& e);
		bool onMousePressed(const MousePressedEvent& e);
		bool onMouseReleased(const MouseReleasedEvent& e);
		bool onMouseScrolled(const MouseScrolledEvent& e);

		uint32_t getMovingButton() const { return m_laptopMode ? BV_MOUSE_BUTTON_LEFT : BV_MOUSE_BUTTON_MIDDLE; }

	private:
		Camera m_camera;

		glm::vec3 m_targetPoint = { 0.0f, 0.0f, 0.0f };

		glm::ivec2 m_currentMousePosition = { 0, 0 };
		glm::ivec2 m_mouseOffset = { 0, 0 };

		float m_angleSensitivity = 0.15f;
		float m_translationSensitivity = m_angleSensitivity / 50.0f;
		float m_distanceFromObject = 2.0f;

		bool m_isViewportHovered = true;
		bool m_isCameraControlled = false;
		bool m_laptopMode = false;
	};

}