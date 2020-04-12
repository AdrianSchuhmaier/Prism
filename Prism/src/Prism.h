#pragma once

#include "Core/Application.h"
#include "Core/TaskSystem/TaskSystem.h"

#include "Util/Log/Log.h"

/** 
 * Client must implement a derived class of Prism::Application and use this Macro to
 * return new Instance();
 *
 * This Instance gets properly destructed by the once the Run() method terminates.
 */
#define APPLICATION_ENTRY_POINT Prism::Application* Prism::CreateApplication()
extern Prism::Application* Prism::CreateApplication();

// Main method is defined here, games using Prism only define the entry point!
int main(int argc, char** argv)
{
	Prism::Log::Init();
	Prism::TaskSystem::Init();

	PR_CORE_INFO("Creating Application");
	Prism::Application* app = Prism::CreateApplication();


	PR_CORE_INFO("Running Application...");
	app->Run();
	PR_CORE_INFO("Closing Application...");


	// other threads may still be running tasks...
	Prism::TaskSystem::Wait();

	delete app;

	// deconstruction may have started new tasks...
	Prism::TaskSystem::Finish();
	return 0;
}