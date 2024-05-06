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
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_width(width)
			, m_height(height)
		{}

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }

		WRITE_TYPE_GETTERS(eventType::windowResize)

	private:
		uint32_t m_width, m_height;
	};

}