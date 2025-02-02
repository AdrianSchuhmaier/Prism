#pragma once

#include "Entities/World.h"
#include "Entities/Entity.h"

#include "Core/Events/Event.h"
#include "Core/Window/Window.h"
#include "Core/Graphics/Renderer.h"

#include"Scripting/Lua.h"

#include <memory>

namespace Prism {

	/******************************************************************************
	 * Main Prism Application
	 * 
	 * Acts as the context: holds the window, runs the main loop, receives events
	 * 
	 * Also manages static(!) systems (ResourceManager, VulkanInstance, ...)
	 * => there must only be one instance of this class!
	 *****************************************************************************/
	class Application {
	public:

		Application(const Window::Properties& props);
		virtual ~Application();

		void Run();
		void EventCallback(Event&);

		virtual void OnUpdate(float dt) = 0;
		virtual void OnEvent(Event&) = 0;

		Lua* GetLuaInstance() { return m_LuaInstance.get(); }

	protected:
		double GetTime() { return m_MainWindow->GetTime(); }
		float GetDeltaTime() { return (float)(GetTime() - m_LastFrameTime); }
		void LimitFPS(float fps) { m_MinFrameDuration = 1.0f / fps; }
		void UnlimitFPS() { m_MinFrameDuration = 0.0f; }
	private:
		void StepFrame() { m_LastFrameTime = GetTime(); }

	protected:
		std::unique_ptr<World> world = nullptr;
	private:
		std::unique_ptr<Lua> m_LuaInstance = nullptr;
		std::unique_ptr<Window> m_MainWindow = nullptr;

		bool m_Running = true;
		bool m_Minimized = false;

		double m_LastFrameTime;
		float m_MinFrameDuration = 0;
	};

	// defined in the client application
	Application* CreateApplication();
}