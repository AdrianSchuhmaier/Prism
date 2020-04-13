#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace Prism {

	struct VulkanSwapchain
	{
		struct SupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			VkSurfaceFormatKHR format;
			VkPresentModeKHR presentMode;
		};

		VkSwapchainKHR swapchain;
		VkFormat format;
		VkExtent2D extent;
		std::optional<VkRenderPass> renderPass = std::nullopt;

		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		VulkanSwapchain(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent,
			const SupportDetails& details, std::optional<VkSwapchainKHR> oldSwapchain = std::nullopt);
		~VulkanSwapchain();

	private:
		void createFrameBuffers();
		VkImageView createImageView(VkImage image);

		VkDevice m_Device;
	};
}