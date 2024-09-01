#pragma once

#include "Event.h"

namespace Brickview
{

	class KeyEvent : public Event
	{
	public:
		inline uint32_t getKeyCode() const { return m_keyCode; }

	protected:
		KeyEvent(uint32_t keyCode)
			: m_keyCode(keyCode)
		{}

	private:
		uint32_t m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(uint32_t keyCode)
			: KeyEvent(keyCode)
		{}

		WRITE_TYPE_GETTERS(eventType::keyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(uint32_t keyCode)
			: KeyEvent(keyCode)
		{}

		WRITE_TYPE_GETTERS(eventType::keyReleased)
	};

}