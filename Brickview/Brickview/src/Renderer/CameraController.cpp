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

	CameraController::CameraController(const glm::vec3& target)
		: m_targetPoint(target)
		, m_currentMousePosition(-1, -1)
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
			const glm::vec3& cameraPosition = m_camera.getPosition();
			float pitch = m_camera.getPitch();
			float yaw = m_camera.getYaw();

			const float sensibility = 0.005f;
			float theta = sensibility * mouseOffsetX;
			float beta = sensibility * mouseOffsetY;

			glm::vec3 lookAt = cameraPosition - m_targetPoint;
			const glm::vec3 rightVector = glm::normalize(glm::cross(lookAt, {0.0f, 1.0f, 0.0f}));

			glm::vec3 newPosition = glm::rotateY(lookAt, -theta);
			newPosition = glm::rotate(newPosition, beta, rightVector);

			float newYaw = yaw - glm::degrees(theta);
			float newPitch = pitch - glm::degrees(beta);

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