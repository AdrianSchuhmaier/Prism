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

		ResourceManager::Init();

		// not really necessary as this is very quick for now
		m_LuaInstance = std::make_unique<Lua>();;

		// window creation must happen on the main thread
		m_MainWindow = std::make_unique<Window>(props);
		m_MainWindow->SetEventCallback(PR_BIND_EVENT_FN(Application::EventCallback));
		
		// can't be done before the first window creation (glfwInit())
		VulkanInstance::Init();

		world = std::make_unique<World>();
		world->systems.Create<Renderer>(m_MainWindow.get());



		m_LastFrameTime = GetTime();
	}

	Application::~Application()
	{
		// invoke destruction of RAII objects
		world = nullptr;
		m_MainWindow = nullptr;
		m_LuaInstance = nullptr;

		VulkanInstance::Shutdown();
		ResourceManager::Shutdown();
	}

	void Application::Run()
	{
		if (auto renderer = world->systems.Get<Renderer>())
		{
		}

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