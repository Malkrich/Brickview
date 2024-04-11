#pragma once

#include "Event.h"

namespace Brickview
{

	class KeyEvent : public Event
	{
	public:
		inline unsigned int getKeyCode() const { return m_keyCode; }

	protected:
		KeyEvent(unsigned int keyCode)
			: m_keyCode(keyCode)
		{}

	private:
		unsigned int m_keyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(unsigned int keyCode)
			: KeyEvent(keyCode)
		{}

		WRITE_TYPE_GETTERS(eventType::keyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(unsigned int keyCode)
			: KeyEvent(keyCode)
		{}

		WRITE_TYPE_GETTERS(eventType::keyReleased)
	};

}