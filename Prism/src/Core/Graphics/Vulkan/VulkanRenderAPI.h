#pragma once

#include "Core/Window/Window.h"
#include "VulkanContext.h"

namespace Prism {

	class VulkanRenderAPI {
	public:
		VulkanRenderAPI(Window* window) : m_Context(window) {}

		VulkanContext* GetContext() { return &m_Context; }

	private:
		VulkanContext m_Context;
	};
}