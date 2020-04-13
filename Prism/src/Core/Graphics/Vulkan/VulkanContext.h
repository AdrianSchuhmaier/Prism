#pragma once

#include "Core/Window/Window.h"
#include "VulkanSwapchain.h"

#include <vulkan/vulkan.h>

namespace Prism {

	class VulkanContext {
	public:
		VulkanContext(Window* window);
		~VulkanContext();

		void Resize();
		void WaitDeviceIdle() { vkDeviceWaitIdle(m_Device); }

	private:
		VkSurfaceKHR createSurface(GLFWwindow* window);
		std::pair<VkPhysicalDevice, VkDevice> selectDevice();
		VulkanSwapchain::SupportDetails gatherSupportDetails();
		VkExtent2D getCurrentWindowExtent();

	private:
		GLFWwindow* m_WindowHandle;

		VkSurfaceKHR m_Surface;
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;

		uint32_t m_GraphicsQueueFamilyIndex;
		uint32_t m_TransferQueueFamilyIndex;
		VkQueue m_GraphicsQueue;
		VkQueue m_TransferQueue;

		VulkanSwapchain::SupportDetails m_SwapchainSupportDetails;
		std::unique_ptr<VulkanSwapchain> m_Swapchain = nullptr;
	};
}