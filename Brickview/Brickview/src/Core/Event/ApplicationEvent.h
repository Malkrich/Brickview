#pragma once

#include "Event.h"

namespace Brickview
{

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		WRITE_TYPE_GETTERS(eventType::windowClose)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width)
			, m_height(height)
		{}

		inline unsigned int getWidth() const { return m_width; }
		inline unsigned int getHeight() const { return m_height; }

		WRITE_TYPE_GETTERS(eventType::windowResize)

	private:
		unsigned int m_width, m_height;
	};

}