#pragma once

#include "Core/Window/Window.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"

#include <vulkan/vulkan.h>

namespace Prism {

	class VulkanContext {
	public:
		VulkanContext(Window* window);
		~VulkanContext();

		void Resize();
		void WaitDeviceIdle() { vkDeviceWaitIdle(m_Device); }

		VkDevice GetDevice() { return m_Device; }
		VulkanSwapchain* GetSwapchain() { return m_Swapchain.get(); }
		VkRenderPass GetDefaultRenderPass() { return m_DefaultRenderPass->GetHandle(); }

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

		std::unique_ptr<VulkanRenderPass> m_DefaultRenderPass = nullptr;
	};
}