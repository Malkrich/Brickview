#pragma once

#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/KeyCodes.h"
#include "PerspectiveCamera.h"

namespace Brickview
{

	struct CameraControllerSpecifications
	{
		// Control parameters
		glm::vec3 TargetPosition = { 0.0f, 0.0f, 0.0f };
		float DistanceFromObject = 2.0f;
		bool LaptopMode = false;

		// Camera parameters
		float Width, Height;
		glm::vec3 CameraPosition = { 0.0f, 0.0f, 1.0f };
		float Pitch = 0.0f, Yaw = 0.0f;

	};

	class CameraController
	{
	public:
		CameraController(const CameraControllerSpecifications& specs);

		const PerspectiveCamera& getCamera() const { return m_camera; }

		void resize(float width, float height);

		void setTargetPoint(const glm::vec3& targetPoint);

		void setViewportHovered(bool hovered) { m_isViewportHovered = hovered; }
		void setLaptopMode(bool laptopMode) { m_laptopMode = laptopMode; }

		void onEvent(Event& e);

	private:
		uint32_t getMovingButton() const { return m_laptopMode ? BV_MOUSE_BUTTON_LEFT : BV_MOUSE_BUTTON_MIDDLE; }

		bool onMouseMoved(const MouseMovedEvent& e);
		bool onMousePressed(const MousePressedEvent& e);
		bool onMouseReleased(const MouseReleasedEvent& e);
		bool onMouseScrolled(const MouseScrolledEvent& e);

		void panCamera(const glm::ivec2& mouseOffset);
		void orbitCamera(const glm::ivec2& mouseOffset);

	private:
		// Camera description
		PerspectiveCamera m_camera;
		glm::vec3 m_targetPoint;
		float m_distanceFromObject;
		float m_width, m_height;

		// Control variables
		glm::ivec2 m_currentMousePosition = { 0, 0 };
		bool m_isViewportHovered = true;
		bool m_isCameraControlled = false;
		bool m_laptopMode = false;

		// Constants
		inline static float s_scrollSensitivity = 0.02f;
		inline static float s_angleSensitivity = 0.15f;
		inline static float s_translationSensitivity = 0.0003f;
	};

}