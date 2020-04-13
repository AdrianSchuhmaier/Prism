#include "VulkanSwapchain.h"

#include "Util/Log/Log.h"

namespace Prism {

	VulkanSwapchain::VulkanSwapchain(VkDevice device, VkSurfaceKHR surface, VkExtent2D extent,
		const SupportDetails& details, std::optional<VkSwapchainKHR> oldSwapchain)
		: m_Device(device), extent(extent), format(details.format.format)
	{
		// image count (minCount + 1 to triplebuffer)
		uint32_t imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
			imageCount = details.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = details.format.format;
		swapchainCreateInfo.imageColorSpace = details.format.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = details.capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = details.presentMode;
		swapchainCreateInfo.clipped = true;
		swapchainCreateInfo.oldSwapchain = oldSwapchain ? oldSwapchain.value() : nullptr;

		vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);

		// get images from Swapchain
		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
		images = std::vector<VkImage>{ swapchainImageCount };
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, images.data());

		imageViews.reserve(images.size());
		for (auto image : images)
			imageViews.push_back(createImageView(image));
		
		// createFrameBuffers(); // uncomment once Renderpasses are set
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		for (auto& framebuffer : framebuffers)
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		framebuffers.clear();

		for (const auto& imageView : imageViews)
			vkDestroyImageView(m_Device, imageView, nullptr);
		imageViews.clear();

		vkDestroySwapchainKHR(m_Device, swapchain, nullptr);
	}

	void VulkanSwapchain::createFrameBuffers()
	{
		PR_CORE_ASSERT(framebuffers.size() < 1, "There should be no Framebuffers at this point");

		//if (!renderPass.has_value()) renderPass = Defaults::GetDefaultRenderPass()->GetHandle();

		framebuffers = std::vector<VkFramebuffer>{ imageViews.size() };
		for (int i = 0; i < imageViews.size(); i++)
		{
			VkImageView attachments[] = {
				imageViews[i] // might be more
			};

			VkFramebufferCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = renderPass.value();
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = extent.width;
			createInfo.height = extent.height;
			createInfo.layers = 1;

			vkCreateFramebuffer(m_Device, &createInfo, nullptr, &framebuffers[i]);
		}
	}

	VkImageView VulkanSwapchain::createImageView(VkImage image)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView result;
		vkCreateImageView(m_Device, &createInfo, nullptr, &result);
		return result;
	}
}