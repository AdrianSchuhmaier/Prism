#pragma once

#include "Core/Window/Window.h"
#include "Systems/System.h"


namespace Prism {

	class VulkanRenderAPI;

	class Renderer : public System {
	public:
		Renderer(Window* window);
		~Renderer();

	private:
		VulkanRenderAPI* m_Renderer;
	};
}