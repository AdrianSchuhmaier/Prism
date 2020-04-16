#pragma once

#include "Core/Window/Window.h"
#include "Systems/System.h"


namespace Prism {

	class VulkanRenderAPI;

	class Renderer : public System {
	public:
		Renderer(Window* window);
		~Renderer();

		/** Waits for all rendering to be finished. */
		void Finish() { /* TODO */ };

	private:
		friend class ResourceManager;
		VulkanRenderAPI* m_Renderer;
	};
}