#pragma once

namespace Brickview
{
	enum class eventType
	{
		windowClose = 0, windowResize
	};

	class Event
	{
	public:
		inline virtual eventType getType() const = 0;

	private:
		bool m_handle = false;

		friend class EventDispatcher;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		inline virtual eventType getType() const { return eventType::windowClose; }
		inline static eventType getTypeStatic() { return eventType::windowClose; }
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width)
			, m_height(height)
		{}

		inline virtual eventType getType() const { return eventType::windowResize; }
		inline static eventType getTypeStatic() { return eventType::windowResize; }

		inline unsigned int getWidth() const { return m_width; }
		inline unsigned int getHeight() const { return m_height; }
	private:
		unsigned int m_width, m_height;
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