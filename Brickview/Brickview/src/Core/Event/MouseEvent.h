#pragma once

#include "Event.h"

namespace Brickview
{

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(unsigned int posX, unsigned int posY)
			: m_posX(posX)
			, m_posY(posY)
		{}

		inline unsigned int getPosX() const { return m_posX; }
		inline unsigned int getPosY() const { return m_posY; }

		WRITE_TYPE_GETTERS(eventType::mouseMoved)

	private:
		unsigned int m_posX, m_posY;
	};


	class MouseButtonEvent : public Event
	{
	public:
		inline int getMouseButton() const { return m_button; }

	protected:
		MouseButtonEvent(int button)
			: m_button(button)
		{}

	private:
		int m_button;
	};

	class MousePressedEvent : public MouseButtonEvent
	{
	public:
		MousePressedEvent(int button)
			: MouseButtonEvent(button)
		{}

		WRITE_TYPE_GETTERS(eventType::mousePressed)
	};

	class MouseReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseReleasedEvent(int button)
			: MouseButtonEvent(button)
		{}

		WRITE_TYPE_GETTERS(eventType::mouseReleased)
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(double offsetX, double offsetY)
			: m_offsetX(offsetX)
			, m_offsetY(offsetY)
		{}

		inline double getOffsetX() const { return m_offsetX; }
		inline double getOffsetY() const { return m_offsetY; }

		WRITE_TYPE_GETTERS(eventType::MouseScrolled)

	private:
		double m_offsetX, m_offsetY;
	};

}