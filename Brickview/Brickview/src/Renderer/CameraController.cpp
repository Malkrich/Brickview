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
			const float pi = glm::pi<float>();
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
		: m_targetPoint(target)
		, m_currentMousePosition(-1)
	{
		// TEMP
		m_camera.setPosition({ 0.0f, 0.0f, 2.0f });
	}

	void CameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowResizeEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onWindowResize));
		dispatcher.dispatch<MouseMoveEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseMove));
		dispatcher.dispatch<MousePressedEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMousePressed));
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
			int mouseOffsetX = e.getPosX() - m_currentMousePosition.x;
			int mouseOffsetY = e.getPosY() - m_currentMousePosition.y;
			m_currentMousePosition.x = e.getPosX();
			m_currentMousePosition.y = e.getPosY();

			const float angleSensitivity = 0.05f;
			const float distanceFromObject = 2.0f;

			float newPitch = m_camera.getPitch() - angleSensitivity * mouseOffsetY;
			float newYaw = m_camera.getYaw() - angleSensitivity * mouseOffsetX;

			glm::vec3 newPosition = Utils::SphericalToCartesian(newPitch, newYaw, distanceFromObject);

			m_camera.setPosition(newPosition);
			m_camera.setRotation(newPitch, newYaw);
		}

		return false;
	}

	bool CameraController::onMousePressed(const MousePressedEvent& e)
	{
		if (e.getMouseButton() == BV_MOUSE_BUTTON_MIDDLE)
			m_currentMousePosition = Input::getMousePosition();

		return false;
	}

}