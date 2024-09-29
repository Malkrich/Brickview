#pragma once

#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/KeyCodes.h"
#include "Camera.h"

#include <tuple>

namespace Brickview
{

	struct CameraControllerSpecifications
	{
		glm::vec3 TargetPosition = { 0.0f, 0.0f, 0.0f };
		float DistanceFromObject = 2.0f;
		bool LaptopMode = false;
	};

	class CameraController
	{
	public:
		CameraController();
		CameraController(const CameraControllerSpecifications& spec);

		const Camera& getCamera() const { return m_camera; }

		void resize(uint32_t width, uint32_t height) { m_camera.setViewportDimension(width, height); }

		// TODO: update position
		void setTargetPoint(const glm::vec3& targetPoint) { m_targetPoint = targetPoint; }

		void setViewportHovered(bool hovered) { m_isViewportHovered = hovered; }
		void setLaptopMode(bool laptopMode) { m_laptopMode = laptopMode; }

		void onEvent(Event& e);

	private:
		typedef glm::vec3 EulerRotation;

	private:
		void updateCameraPositionAndRotation(const glm::vec3& newPosition, const EulerRotation& rotation)
		{
			m_camera.setPosition(newPosition);
			m_camera.setRotation(rotation.x, rotation.y);
		}
		void updateCameraPosition(const glm::vec3& newPosition)
		{
			m_camera.setPosition(newPosition);
		}

		glm::vec3 computeTranslationOffset(const glm::ivec2& mouseOffset) const;
		std::tuple<glm::vec3, EulerRotation> computeNewRotationAndTranslation(const glm::ivec2& mouseOffset) const;

		bool onMouseMoved(const MouseMovedEvent& e);
		bool onMousePressed(const MousePressedEvent& e);
		bool onMouseReleased(const MouseReleasedEvent& e);
		bool onMouseScrolled(const MouseScrolledEvent& e);

		uint32_t getMovingButton() const { return m_laptopMode ? BV_MOUSE_BUTTON_LEFT : BV_MOUSE_BUTTON_MIDDLE; }

	private:
		// Camera description
		Camera m_camera;
		glm::vec3 m_targetPoint;
		float m_distanceFromObject;

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