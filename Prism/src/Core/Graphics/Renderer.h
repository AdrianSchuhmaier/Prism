#pragma once

#include "Core/Window/Window.h"

namespace Prism {

	class VulkanRenderAPI;

	class Renderer {
	public:
		Renderer(Window* window);
		~Renderer();

	private:
		VulkanRenderAPI* m_Renderer;
	};
}