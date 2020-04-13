#include "VulkanContext.h"
#include "VulkanInstance.h"

#include <GLFW/glfw3.h>
#include <map>

namespace Prism {
	const auto swapchainEXT = "VK_KHR_swapchain";

	// SupportDetails don't change during Swapchain recreation, so it's handled by the VulkanContext
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);


	VulkanContext::VulkanContext(Window* window)
		: m_WindowHandle(window->GetWindowHandle())
	{
		m_Surface = createSurface(m_WindowHandle);
		std::tie(m_PhysicalDevice, m_Device) = selectDevice(); // also configures queues

		m_SwapchainSupportDetails = gatherSupportDetails(); // used for every swapchain creations
		m_Swapchain = std::make_unique<VulkanSwapchain>( m_Device, m_Surface, getCurrentWindowExtent(), m_SwapchainSupportDetails );
	}

	VulkanContext::~VulkanContext()
	{
		vkDeviceWaitIdle(m_Device);

		m_Swapchain = nullptr; // destruct manually before destroying device and surface

		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(VulkanInstance::Get(), m_Surface, nullptr);
	}

	void VulkanContext::Resize()
	{
		auto newSwapchain = std::make_unique<VulkanSwapchain>(m_Device, m_Surface, getCurrentWindowExtent(),
			m_SwapchainSupportDetails, m_Swapchain->swapchain); // also provide current one

		// wait only when the new swapchain has been created
		vkDeviceWaitIdle(m_Device);

		m_Swapchain.swap(newSwapchain);
	}

	VkSurfaceKHR VulkanContext::createSurface(GLFWwindow* window)
	{
		VkSurfaceKHR result;
		auto res = glfwCreateWindowSurface(VulkanInstance::Get(), window, nullptr, &result);
		PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create Vulkan window surface!");

		return result;
	}

	std::pair<VkPhysicalDevice, VkDevice> VulkanContext::selectDevice()
	{
		uint32_t physicalDeviceCount;
		vkEnumeratePhysicalDevices(VulkanInstance::Get(), &physicalDeviceCount, nullptr);
		PR_CORE_ASSERT(physicalDeviceCount > 0, "No physical device found!");
		std::vector<VkPhysicalDevice> physicalDevices{ physicalDeviceCount };
		vkEnumeratePhysicalDevices(VulkanInstance::Get(), &physicalDeviceCount, physicalDevices.data());

		std::map<int, VkPhysicalDevice, std::less<int>> physicalDeviceRanking{};
		for (const auto& physicalDevice : physicalDevices)
		{
			// define check-function
			auto checkSwapchainSupport = [](const auto& p) {
				uint32_t propertyCount;
				vkEnumerateDeviceExtensionProperties(p, nullptr, &propertyCount, nullptr);
				std::vector<VkExtensionProperties> properties{ propertyCount };
				vkEnumerateDeviceExtensionProperties(p, nullptr, &propertyCount, properties.data());

				for (const auto& property : properties)
					if (strcmp(property.extensionName, swapchainEXT) == 0)
						return true;

				return false;
			};
			if (!checkSwapchainSupport(physicalDevice)) continue;

			// evaluate properties
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevice, &props);

			int score =
				// affects image quality
				props.limits.maxImageDimension2D
				// bonus for discrete GPU (performance)
				+ 4000 * (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);

			physicalDeviceRanking.insert({ score, physicalDevice });
		}

		// loop through ranked devices and try to configure queues
		for (const auto& [_, physicalDevice] : physicalDeviceRanking)
		{
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies{ queueFamilyCount };
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

			m_GraphicsQueueFamilyIndex = m_TransferQueueFamilyIndex = queueFamilyCount;

			// find graphics queue family
			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				const auto& queueFamily = queueFamilies[i];
				VkBool32 surfaceSupported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &surfaceSupported);
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && surfaceSupported)
				{
					m_GraphicsQueueFamilyIndex = i;
					break;
				}
			}
			if (m_GraphicsQueueFamilyIndex >= queueFamilyCount) continue;

			// try to find separate transfer queue family
			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				const auto& queueFamily = queueFamilies[i];
				if (i != m_GraphicsQueueFamilyIndex && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					m_TransferQueueFamilyIndex = i;
					break;
				}
			}

			uint32_t transferQueueIndex = 0;
			// if no separate queue family found, take graphics family but with different queue if possible
			if (m_TransferQueueFamilyIndex >= queueFamilyCount)
			{
				m_TransferQueueFamilyIndex = m_GraphicsQueueFamilyIndex;
				if (queueFamilies[m_TransferQueueFamilyIndex].queueCount > 1)
					transferQueueIndex = 1;
				else continue;
			}

			// used for configuring queueCount and queueIndex below
			const uint32_t sameQueueFamily = m_TransferQueueFamilyIndex == m_GraphicsQueueFamilyIndex;

			const auto priorities = std::vector{ 0.0f, 0.0f };
			VkDeviceQueueCreateInfo queueCreateInfos[2]{};
			queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[0].queueFamilyIndex = m_GraphicsQueueFamilyIndex;
			queueCreateInfos[0].queueCount = 1 + sameQueueFamily; // 1 if separate families else 2
			queueCreateInfos[0].pQueuePriorities = priorities.data();
			// define 2nd struct, may simply be ignored later
			queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[1].queueFamilyIndex = m_TransferQueueFamilyIndex;
			queueCreateInfos[1].queueCount = 1;
			queueCreateInfos[1].pQueuePriorities = priorities.data();

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = 2 - sameQueueFamily; // same trick as above
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(useValidation); // see VulkanInstance.h
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data(); // see VulkanInstance.h
			deviceCreateInfo.enabledExtensionCount = 1;
			deviceCreateInfo.ppEnabledExtensionNames = &swapchainEXT;
			deviceCreateInfo.pEnabledFeatures = nullptr;

			VkDevice device;
			auto res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
			PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create logical device!");

			// finally retreive configured queues
			vkGetDeviceQueue(device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
			vkGetDeviceQueue(device, m_TransferQueueFamilyIndex, sameQueueFamily, &m_GraphicsQueue);

			return { physicalDevice, device };
		}

		PR_CORE_ASSERT(false, "No suitable device found!");
		return {};
	}

	VulkanSwapchain::SupportDetails VulkanContext::gatherSupportDetails()
	{
		VulkanSwapchain::SupportDetails result{};
		// get surfaceCapabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &result.capabilities);

		// get surfaceFormats
		uint32_t surfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr);
		auto formats = std::vector<VkSurfaceFormatKHR>{ surfaceFormatCount };
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, formats.data());
		PR_CORE_ASSERT(!formats.empty(), "No formats supported.");
		// choose one
		result.format = chooseSwapSurfaceFormat(formats);

		// get presentModes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
		auto presentModes = std::vector<VkPresentModeKHR>{ presentModeCount };
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());
		PR_CORE_ASSERT(!presentModes.empty(), "No present modes supported.");
		// choose one
		result.presentMode = chooseSwapPresentMode(presentModes);

		return result;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		PR_CORE_ASSERT(availableFormats.size() != 1 || availableFormats[0].format != VK_FORMAT_UNDEFINED, "Surface format undefined!");

		for (const auto& availableFormat : availableFormats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : availablePresentModes)
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				bestMode = availablePresentMode;

		return bestMode;
	}

	VkExtent2D VulkanContext::getCurrentWindowExtent()
	{
		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);
		return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}
}