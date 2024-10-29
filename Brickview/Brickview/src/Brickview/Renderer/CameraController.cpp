#include "Pch.h"
#include "CameraController.h"
#include "Core/Input.h"

#include "Utils/MathUtils.h"
#include "Metric/World.h"

namespace Brickview
{

	CameraController::CameraController(const CameraControllerSpecifications& specs)
		: m_camera(specs.Width, specs.Height, specs.CameraPosition, specs.Pitch, specs.Yaw)
		, m_currentMousePosition(-1)
		, m_targetPoint(specs.TargetPosition)
		, m_distanceFromObject(specs.DistanceFromObject)
		, m_laptopMode(specs.LaptopMode)
		, m_width(specs.Width)
		, m_height(specs.Height)
	{
	}

	void CameraController::resize(float width, float height)
	{
		if (width != m_width || height != m_height)
		{
			m_camera.setViewportDimensions(width, height);
			m_width  = width;
			m_height = height;
		}
	}

	void CameraController::setTargetPoint(const glm::vec3& targetPoint)
	{
		// Update camera position
		glm::vec3 translation = targetPoint - m_targetPoint;
		glm::vec3 newCameraPosition = m_camera.getPosition() + translation;
		m_camera.setPosition(newCameraPosition);

		// Set new target point
		m_targetPoint = targetPoint;
	}

	void CameraController::onUpdate()
	{
		if (Input::isMouseButtonPressed(getMovingButton()))
		{
			glm::ivec2 newMousePosition = Input::getMousePosition();
			glm::ivec2 mouseOffset = newMousePosition - m_currentMousePosition;
			m_currentMousePosition = newMousePosition;

			if (Input::isKeyPressed(BV_KEY_LEFT_SHIFT) || Input::isKeyPressed(BV_KEY_RIGHT_SHIFT))
				panCamera(mouseOffset);
			else
				orbitCamera(mouseOffset);
		}
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

		m_distanceFromObject  -= (float)e.getOffsetY() * s_scrollSensitivity;
		glm::vec3 newPosition  = MathUtils::SphericalToCartesian(m_camera.getPitch(), m_camera.getYaw(), m_distanceFromObject);
		newPosition           += m_targetPoint;

		m_camera.setPosition(newPosition);

		return false;
	}

	void CameraController::orbitCamera(const glm::ivec2& mouseOffset)
	{
		// The dot product gives a negative value when the camera is looking below the object
		float yawOffsetSense = glm::dot(World::getUpVector(), m_camera.getUpVector()) > 0.0f ? 1.0f : -1.0f;
		float newPitch = m_camera.getPitch() - s_angleSensitivity * mouseOffset.y;
		float newYaw = m_camera.getYaw() - s_angleSensitivity * mouseOffset.x * yawOffsetSense;

		glm::vec3 newPosition = MathUtils::SphericalToCartesian(newPitch, newYaw, m_distanceFromObject);
		newPosition += m_targetPoint;

		m_camera.setPosition(newPosition);
		m_camera.setRotation(newPitch, newYaw);
	}

	void CameraController::panCamera(const glm::ivec2& mouseOffset)
	{
		glm::vec3 translation = glm::vec3(0.0f);
		translation -= m_camera.getRightVector() * (float)mouseOffset.x * s_translationSensitivity;
		translation += m_camera.getUpVector() * (float)mouseOffset.y * s_translationSensitivity;

		glm::vec3 newPosition = m_camera.getPosition() + translation;
		m_targetPoint += translation;

		m_camera.setPosition(newPosition);
	}

}