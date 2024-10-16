#include "Pch.h"
#include "CameraController.h"
#include "Core/Input.h"

#include "Utils/MathUtils.h"
#include "Metric/World.h"

namespace Brickview
{

	CameraController::CameraController()
		: CameraController(CameraControllerSpecifications())
	{}

	CameraController::CameraController(const CameraControllerSpecifications& spec)
		: m_camera({ (float)m_width, (float)m_height }, { 0.0f, 0.0f, spec.DistanceFromObject } )
		, m_currentMousePosition(-1)
		, m_targetPoint(spec.TargetPosition)
		, m_distanceFromObject(spec.DistanceFromObject)
		, m_laptopMode(spec.LaptopMode)
		, m_width(spec.Width)
		, m_height(spec.Height)
	{
	}

	glm::vec3 CameraController::computeTranslationOffset(const glm::ivec2& mouseOffset) const
	{
		glm::vec3 translation(0.0f);
		translation -= m_camera.getRightVector() * (float)mouseOffset.x * s_translationSensitivity;
		translation += m_camera.getUpVector() * (float)mouseOffset.y * s_translationSensitivity;
		return translation;
	}

	std::tuple<glm::vec3, CameraController::EulerRotation> CameraController::computeNewRotationAndTranslation(const glm::ivec2& mouseOffset) const
	{
		// The dot product gives a negative value when the camera is looking below the object
		float yawOffsetSense = glm::dot(World::getUpVector(), m_camera.getUpVector()) > 0.0f ? 1.0f : -1.0f;

		EulerRotation newRotation(0.0f);
		newRotation.x         = m_camera.getPitch() - s_angleSensitivity * mouseOffset.y;
		newRotation.y         = m_camera.getYaw() - s_angleSensitivity * mouseOffset.x * yawOffsetSense;
		glm::vec3 newPosition = MathUtils::SphericalToCartesian(newRotation.x, newRotation.y, m_distanceFromObject);
		newPosition          += m_targetPoint;

		return { newPosition, newRotation };
	}

	void CameraController::resize(uint32_t width, uint32_t height)
	{
		if (width != m_width || height != m_height)
		{
			m_camera.setViewportDimension(width, height);
			m_width  = width;
			m_height = height;
		}
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

		glm::ivec2 mouseOffset(0);
		mouseOffset.x = e.getPosX() - m_currentMousePosition.x;
		mouseOffset.y = e.getPosY() - m_currentMousePosition.y;
		m_currentMousePosition.x = e.getPosX();
		m_currentMousePosition.y = e.getPosY();

		glm::vec3 newPosition     = m_camera.getPosition();
		EulerRotation newRotation = { m_camera.getPitch(), m_camera.getYaw(), 0.0f };

		if (Input::isKeyPressed(BV_KEY_LEFT_SHIFT) || Input::isKeyPressed(BV_KEY_RIGHT_SHIFT))
		{
			glm::vec3 translationOffset = computeTranslationOffset(mouseOffset);
			newPosition   += translationOffset;
			m_targetPoint += translationOffset;
		}
		else
		{
			auto [position, rotation] = computeNewRotationAndTranslation(mouseOffset);
			newPosition = position;
			newRotation = rotation;
		}
	
		updateCameraPositionAndRotation(newPosition, newRotation);

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

		m_distanceFromObject  -= (float)e.getOffsetY() * s_scrollSensitivity;
		glm::vec3 newPosition  = MathUtils::SphericalToCartesian(m_camera.getPitch(), m_camera.getYaw(), m_distanceFromObject);
		newPosition           += m_targetPoint;

		updateCameraPosition(newPosition);

		return false;
	}
}