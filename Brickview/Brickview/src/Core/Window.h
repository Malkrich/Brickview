#pragma once

#include "Core/Event/Event.h"

struct GLFWwindow;

namespace Brickview
{

	class Window
	{
	public:
		struct WindowSettings
		{
		public:
			WindowSettings()
				: name("Window")
				, width(1280)
				, height(720)
			{}

			WindowSettings(const std::string& nameParam, uint32_t widthParam, uint32_t heightParam)
				: name(nameParam)
				, width(widthParam), height(heightParam)
			{}

		public:
			std::string name;

			uint32_t width;
			uint32_t height;

			EventDispatcher::EventCallbackFn callbackFn;
		};

	public:
		Window(const WindowSettings& windowSettings);

		~Window();

		uint32_t getWidth() const { return m_settings.width; }
		uint32_t getHeight() const { return m_settings.height; }

		void setEventCallbackFunction(const EventDispatcher::EventCallbackFn& callbackFn)
		{
			m_settings.callbackFn = callbackFn;
		}

		void* getLibWindow() const { return (void*)m_window; }

		void onUpdate();

	private:
		void initialize(const WindowSettings& windowSettings);

	private:
		WindowSettings m_settings;

		GLFWwindow* m_window;
	};
}