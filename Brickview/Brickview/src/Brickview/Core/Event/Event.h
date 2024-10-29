#pragma once

namespace Brickview
{
	#define WRITE_EVENT_TYPE(type) virtual EventType getType() const override { return type; } \
								   static  EventType getTypeStatic() { return type; } \

	#define WRITE_EVENT_CATEGORY(category) virtual uint32_t getCategoryFlags() const override { return category; }

	enum class EventType
	{
		WindowClose = 0, WindowResize,
		MouseMoved, MousePressed, MouseReleased, MouseScrolled,
		KeyPressed, KeyReleased
	};

	enum EventCategory : uint32_t
	{
		EventCategoryNone        = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput       = 1 << 1,
		EventCategoryKeyboard    = 1 << 2,
		EventCategoryMouse       = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

	class Event
	{
	public:
		virtual EventType getType() const = 0;
		virtual uint32_t getCategoryFlags() const = 0;

		bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
		}

		bool isHandled() const { return m_isHandle; }
		void setIsHandle(bool isHandle) { m_isHandle = isHandle; }

	private:
		bool m_isHandle = false;

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
			if (!m_event.m_isHandle && m_event.getType() == T::getTypeStatic())
			{
				m_event.m_isHandle = callbackFunction(*((T*)&m_event));
			}
		}

	private:
		Event& m_event;
	};
}