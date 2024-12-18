#pragma once

#include "Event/Event.h"
#include "Renderer/GraphicsContext.h"

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
				: Name("Window")
				, Width(1280)
				, Height(720)
				, VSync(true)
			{}

			WindowSettings(const std::string& nameParam, uint32_t widthParam, uint32_t heightParam, bool vsync = true)
				: Name(nameParam)
				, Width(widthParam)
				, Height(heightParam)
				, VSync(vsync)
			{}

		public:
			std::string Name;

			uint32_t Width;
			uint32_t Height;

			bool VSync = true;

			EventDispatcher::EventCallbackFn CallbackFn;
		};

	public:
		Window(const WindowSettings& windowSettings);

		~Window();

		uint32_t getWidth() const { return m_settings.Width; }
		uint32_t getHeight() const { return m_settings.Height; }

		void setEventCallbackFunction(const EventDispatcher::EventCallbackFn& callbackFn)
		{
			m_settings.CallbackFn = callbackFn;
		}

		void* getLibWindow() const { return (void*)m_window; }

		bool isVSync() const { return m_settings.VSync; }
		void setVSync(bool enable);

		void onUpdate();

	private:
		void initialize(const WindowSettings& windowSettings);

	private:
		GLFWwindow* m_window;

		Scope<GraphicsContext> m_context;

		WindowSettings m_settings;
	};
}