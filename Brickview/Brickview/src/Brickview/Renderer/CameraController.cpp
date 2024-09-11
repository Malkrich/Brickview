#include "Pch.h"
#include "CameraController.h"
#include "Core/Input.h"

#include "Utils/MathUtils.h"
#include "Metric/World.h"

namespace Brickview
{

	namespace Utils
	{
	}

	CameraController::CameraController()
	{
		updatePosition();
	}

	CameraController::CameraController(const CameraControllerSpecifications& spec)
		: m_currentMousePosition(-1)
		, m_targetPoint(spec.TargetPosition)
		, m_laptopMode(spec.LaptopMode)
	{
		updatePosition();
	}

	void CameraController::updatePosition()
	{
		float direction = glm::dot(World::getUpVector(), m_camera.getUpVector()) > 0.0f ? 1.0f : -1.0f;

		float newPitch = m_camera.getPitch() - m_angleSensitivity * m_mouseOffset.y;
		float newYaw = m_camera.getYaw() - m_angleSensitivity * m_mouseOffset.x * direction;

		glm::vec3 newPosition = MathUtils::SphericalToCartesian(newPitch, newYaw, m_distanceFromObject);
		newPosition += m_targetPoint;

		m_camera.setPosition(newPosition);
		m_camera.setRotation(newPitch, newYaw);

		// reseting mouse offset to prevent the update of the orientation
		m_mouseOffset = { 0, 0 };
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

		m_mouseOffset.x = e.getPosX() - m_currentMousePosition.x;
		m_mouseOffset.y = e.getPosY() - m_currentMousePosition.y;
		m_currentMousePosition.x = e.getPosX();
		m_currentMousePosition.y = e.getPosY();

		if (Input::isKeyPressed(BV_KEY_LEFT_SHIFT) || Input::isKeyPressed(BV_KEY_RIGHT_SHIFT))
		{
			glm::vec3 translation(0.0f);
			translation   -= m_camera.getRightVector() * (float)m_mouseOffset.x * m_translationSensitivity;
			translation   += m_camera.getUpVector() * (float)m_mouseOffset.y * m_translationSensitivity;
			m_targetPoint += translation;

			// Reseting offsets to prevent rotations when calling updatePosition()
			m_mouseOffset = { 0, 0 };
		}

		updatePosition();

		return false;
	}

	bool CameraController::onMousePressed(const MousePressedEvent& e)
	{
		uint32_t movingButton = getMovingButton();

		if (e.getMouseButton() == movingButton && m_isViewportHovered)
		{
			m_currentMousePosition = Input::getMousePosition();
			m_isCameraControlled = true;
		}

		return false;
	}

	bool CameraController::onMouseReleased(const MouseReleasedEvent& e)
	{
		uint32_t movingButton = getMovingButton();

		if (e.getMouseButton() == movingButton)
			m_isCameraControlled = false;

		return false;
	}

	bool CameraController::onMouseScrolled(const MouseScrolledEvent& e)
	{
		if (!m_isViewportHovered)
			return false;

		static const float scrollSensitivity = 0.05f;

		m_distanceFromObject -= (float)e.getOffsetY() * scrollSensitivity;

		updatePosition();

		return false;
	}
}