#pragma once

#include <string>

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

			WindowSettings(const std::string& nameParam, unsigned int widthParam, unsigned int heightParam)
				: name(nameParam)
				, width(widthParam), height(heightParam)
			{}

		public:
			std::string name;

			unsigned int width;
			unsigned int height;

			EventDispatcher::EventCallbackFn callbackFn;
		};

	public:
		Window(const WindowSettings& windowSettings);

		~Window();

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