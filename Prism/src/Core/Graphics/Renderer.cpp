#include "Renderer.h"

#include "Vulkan/VulkanRenderAPI.h"

namespace Prism {
	
	Renderer::Renderer(Window* window)
	{
		m_Renderer = new VulkanRenderAPI(window);
	}

	Renderer::~Renderer()
	{
		delete m_Renderer;
	}
}