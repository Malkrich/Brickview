#pragma once

namespace Brickview
{
	#define WRITE_TYPE_GETTERS(type) inline virtual eventType getType() const override { return type; } \
									 inline static eventType getTypeStatic() { return type; }

	enum class eventType
	{
		windowClose = 0, windowResize,
		mouseMoved, mousePressed, mouseReleased, MouseScrolled,
		keyPressed, keyReleased
	};

	class Event
	{
	public:
		inline virtual eventType getType() const = 0;

		inline bool isHandled() const { return m_handle; }

	private:
		bool m_handle = false;

		friend class EventDispatcher;
	};

	class EventDispatcher
	{
	public:
		typedef std::function<void(Event&)> EventCallbackFn;

	public:
		EventDispatcher(Event& e)
			: m_event(e)
		{}

		template<typename T>
		void dispatch(std::function<bool(T&)> callbackFunction)
		{
			if (!m_event.m_handle && m_event.getType() == T::getTypeStatic())
			{
				m_event.m_handle = callbackFunction(*((T*)&m_event));
			}
		}

	private:
		Event& m_event;
	};
}