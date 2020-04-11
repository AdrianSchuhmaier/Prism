#include "Application.h"

#include "Core/Resources/ResourceManager.h"
#include "Core/TaskSystem/TaskSystem.h"
#include "Util/Log/Log.h"


namespace Prism {

	Application* g_Application = nullptr;

	Application::Application(const Window::Properties& props)
	{
		Task{ ResourceManager::Init }.Submit();
		Task{ [&]() { m_LuaInstance = std::make_unique<Lua>(); } }.Submit();

		PR_CORE_ASSERT(!g_Application, "There is already an Application instance!");
		g_Application = this; // set static for global access

		TaskSystem::Wait();
	}

	Application::~Application()
	{
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
	}

	void Application::EventCallback(Event&)
	{
	}
}