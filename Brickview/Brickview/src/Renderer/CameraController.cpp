#include "Pch.h"
#include "CameraController.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"

namespace Brickview
{

	namespace Utils
	{
		static glm::vec3 SphericalToCartesian(float pitch, float yaw, float distance)
		{
			float radPitch = glm::radians(pitch);
			float radYaw   = glm::radians(yaw);
			constexpr float pi = glm::pi<float>();
			glm::vec3 position =
			{
				glm::cos(-radPitch) * glm::cos(pi/2.0f - radYaw),
				glm::sin(-radPitch),
				glm::cos(-radPitch) * glm::sin(pi/2.0f - radYaw)
			};
			position *= distance;
			return position;
		}
	}

	CameraController::CameraController(const glm::vec3& target)
		: m_currentMousePosition(-1)
	{
		updatePosition();
	}

	void CameraController::updatePosition()
	{
		float newPitch = m_camera.getPitch() - m_angleSensitivity * m_mouseOffset.y;
		float newYaw = m_camera.getYaw() - m_angleSensitivity * m_mouseOffset.x;

		glm::vec3 newPosition = Utils::SphericalToCartesian(newPitch, newYaw, m_distanceFromObject);

		m_camera.setPosition(newPosition);
		m_camera.setRotation(newPitch, newYaw);
	}

	void CameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onWindowResize));
		dispatcher.dispatch<MouseMoveEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseMove));
		dispatcher.dispatch<MousePressedEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMousePressed));
		dispatcher.dispatch<MouseScrolledEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseScrolled));
	}

	bool CameraController::onWindowResize(const WindowResizeEvent& e)
	{
		m_camera.setViewportDimension(e.getWidth(), e.getHeight());
		return false;
	}

	bool CameraController::onMouseMove(const MouseMoveEvent& e)
	{
		if(Input::isMouseButtonPressed(BV_MOUSE_BUTTON_MIDDLE))
		{
			m_mouseOffset.x = e.getPosX() - m_currentMousePosition.x;
			m_mouseOffset.y = e.getPosY() - m_currentMousePosition.y;
			m_currentMousePosition.x = e.getPosX();
			m_currentMousePosition.y = e.getPosY();

			updatePosition();
		}

		return false;
	}

	bool CameraController::onMousePressed(const MousePressedEvent& e)
	{
		if (e.getMouseButton() == BV_MOUSE_BUTTON_MIDDLE)
			m_currentMousePosition = Input::getMousePosition();

		return false;
	}

	bool CameraController::onMouseScrolled(const MouseScrolledEvent& e)
	{
		static const double scrollSensitivity = 0.3;

		m_distanceFromObject -= e.getOffsetY() * scrollSensitivity;

		updatePosition();

		return false;
	}
}