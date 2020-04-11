#pragma once

#include <string>
#include <functional>

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

	private:

		// Data struct for GLFW callbacks
		struct WindowData
		{
			Properties& properties;
		};

		void SetGLFWCallbacks();


	public:
		Window(const Properties&);
		~Window() {}

		void OnUpdate();
		double GetTime() const;

		void* GetGLFWWindowHandle() const { return m_WindowHandle; }


	private:
		Properties m_Properties;
		WindowData m_WindowData;

		void* m_WindowHandle = nullptr; // GLFWwindow*

		static bool s_Initialized;
	};
}