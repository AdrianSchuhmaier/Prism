#pragma once

#include "Core/Window/Window.h"

#include <vulkan/vulkan.h>

namespace Prism {

	class VulkanContext {
	public:
		VulkanContext(Window* window);
		~VulkanContext();

	private:
		VkSurfaceKHR m_Surface;
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;
	};
}