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
		newPosition += m_targetPoint;

		m_camera.setPosition(newPosition);
		m_camera.setRotation(newPitch, newYaw);
	}

	void CameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<MouseMovedEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseMoved));
		dispatcher.dispatch<MousePressedEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMousePressed));
		dispatcher.dispatch<MouseReleasedEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseReleased));
		dispatcher.dispatch<MouseScrolledEvent>(BV_BIND_EVENT_FUNCTION(CameraController::onMouseScrolled));
	}

	bool CameraController::onMouseMoved(const MouseMovedEvent& e)
	{
		if (!m_isCameraControlled)
			return false;

		if(Input::isMouseButtonPressed(BV_MOUSE_BUTTON_MIDDLE))
		{
			m_mouseOffset.x = e.getPosX() - m_currentMousePosition.x;
			m_mouseOffset.y = e.getPosY() - m_currentMousePosition.y;
			m_currentMousePosition.x = e.getPosX();
			m_currentMousePosition.y = e.getPosY();

			if (Input::isKeyPressed(BV_KEY_LEFT_SHIFT) || Input::isKeyPressed(BV_KEY_RIGHT_SHIFT))
			{
				glm::vec3 lookAt = m_targetPoint - m_camera.getPosition();
				glm::vec3 up = { 0.0f, 1.0f, 0.0f };
				glm::vec3 right = glm::normalize( glm::cross(up, lookAt) );
				up = glm::normalize( glm::cross(lookAt, right) );

				glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
				translation += right * (float)m_mouseOffset.x;
				translation += up * (float)m_mouseOffset.y;
				translation *= m_translationSensitivity;
				m_targetPoint += translation;

				// reseting mouse offset to prevent the update of the orientation
				m_mouseOffset = { 0, 0 };
			}

			updatePosition();
		}

		return false;
	}

	bool CameraController::onMousePressed(const MousePressedEvent& e)
	{
		if (e.getMouseButton() == BV_MOUSE_BUTTON_MIDDLE && m_isViewportHovered)
		{
			m_currentMousePosition = Input::getMousePosition();
			m_isCameraControlled = true;
		}

		return false;
	}

	bool CameraController::onMouseReleased(const MouseReleasedEvent& e)
	{
		if (e.getMouseButton() == BV_MOUSE_BUTTON_MIDDLE)
			m_isCameraControlled = false;

		return false;
	}

	bool CameraController::onMouseScrolled(const MouseScrolledEvent& e)
	{
		if (!m_isViewportHovered)
			return false;

		static const double scrollSensitivity = 0.3;

		m_distanceFromObject -= e.getOffsetY() * scrollSensitivity;

		updatePosition();

		return false;
	}
}