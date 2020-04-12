#include "Application.h"

#include "Core/TaskSystem/TaskSystem.h"
#include "Core/Resources/ResourceManager.h"

#include "Core/Graphics/Renderer.h"
#include "Core/Graphics/Vulkan/VulkanInstance.h"

#include "Util/Log/Log.h"


namespace Prism {

	Application* g_Application = nullptr;

	Application::Application(const Window::Properties& props)
	{
		PR_CORE_ASSERT(!g_Application, "There is already an Application instance!");
		g_Application = this; // set static for global access

		// TODO: maybe introduce tasks to speed it up?

		ResourceManager::Init();
		VulkanInstance::Init();

		m_LuaInstance = std::make_unique<Lua>();;
		m_MainWindow = std::make_unique<Window>(props);
		m_MainWindow->SetEventCallback(PR_BIND_EVENT_FN(Application::EventCallback));

		m_LastFrameTime = GetTime();
	}

	Application::~Application()
	{
		VulkanInstance::Shutdown();
		ResourceManager::Shutdown();
	}

	Application* Application::Get()
	{
		if (g_Application)
			return g_Application;

		// you should never get here!
		PR_CORE_ASSERT(false, "static Application::Get() must not be called now!");
		return nullptr;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			auto dt = GetDeltaTime();
			m_MainWindow->OnUpdate();

			if (dt >= m_MinFrameDuration && !m_Minimized)
			{
				/*clientApp->*/OnUpdate(dt);

				StepFrame();
			}
		}
	}

	void Application::EventCallback(Event& event)
	{
		event.Handle<WindowCloseEvent>([&](WindowCloseEvent& e)
			{
				PR_CORE_TRACE("WindowClose handled by Application");
				m_Running = false;
				return true;
			});

		event.Handle<WindowResizeEvent>([&](WindowResizeEvent& e)
			{
				if (e.GetWidth() <= 0 || e.GetHeight() <= 0)
				{
					m_Minimized = true;
					return true;
				}
				//Renderer::Resize(e.GetWidth(), e.GetHeight());
				m_Minimized = false;
				return true;
			});
		// Unhandled events: invoke OnEvent on the client application
		if (!event.handled) /*clientApp->*/OnEvent(event);
	}
}