#pragma once

#include "Core/Events/WindowEvent.h"

#include "Util/Log/Log.h"

#include <string>
#include <functional>

// Avoid including GLFW here, as this would make it
// a dependency for other projects using Prism
struct GLFWwindow;

namespace Prism {

	class Window
	{
	public:
		struct Properties
		{
			std::string title = "Prism";
			uint32_t width = 1280, height = 720;
			bool fullscreen = false;
			bool resizable = true;
		};

		using EventCallbackFn = std::function<void(Event&)>;

	private:

		// Data struct for GLFW callbacks
		struct WindowData
		{
			Properties& properties;
			EventCallbackFn callback = [](Event&) {
				PR_CORE_WARN("No window callback set");
			};
		};

		void SetGLFWCallbacks();


	public:
		Window(const Properties&);
		~Window();

		void OnUpdate();
		double GetTime() const;

		void SetEventCallback(const EventCallbackFn&);

		GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }


	private:
		Properties m_Properties;
		WindowData m_WindowData;

		GLFWwindow* m_WindowHandle = nullptr;

		static bool s_Initialized;
	};
}