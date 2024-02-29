#pragma once

#include "Event.h"

namespace Brickview
{

	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(unsigned int posX, unsigned int posY)
			: m_posX(posX)
			, m_posY(posY)
		{}

		inline unsigned int getPosX() const { return m_posX; }
		inline unsigned int getPosY() const { return m_posY; }

		WRITE_TYPE_GETTERS(eventType::mouseMove)

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

	class MouseReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseReleaseEvent(int button)
			: MouseButtonEvent(button)
		{}

		WRITE_TYPE_GETTERS(eventType::mouseRelease)
	};

}