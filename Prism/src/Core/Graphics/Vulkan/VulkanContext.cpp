#include "VulkanContext.h"
#include "VulkanInstance.h"

#include <GLFW/glfw3.h>
#include <map>

namespace Prism {
	const auto swapchainEXT = "VK_KHR_swapchain";

	VkSurfaceKHR createSurface(GLFWwindow* window);
	std::pair<VkPhysicalDevice, VkDevice> selectPhysicalDevice(VkSurfaceKHR surface);


	VulkanContext::VulkanContext(Window* window)
	{
		m_Surface = createSurface(window->GetWindowHandle());
		std::tie(m_PhysicalDevice, m_Device) = selectPhysicalDevice(m_Surface);
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(VulkanInstance::Get(), m_Surface, nullptr);
	}


	VkSurfaceKHR createSurface(GLFWwindow* window)
	{
		VkSurfaceKHR result;
		auto res = glfwCreateWindowSurface(VulkanInstance::Get(), window, nullptr, &result);
		PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create Vulkan window surface!");

		return result;
	}

	std::pair<VkPhysicalDevice, VkDevice> selectPhysicalDevice(VkSurfaceKHR surface)
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

			uint32_t graphicsQueueFamilyIndex, transferQueueFamilyIndex;
			graphicsQueueFamilyIndex = transferQueueFamilyIndex = queueFamilyCount;

			// find graphics queue family
			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				const auto& queueFamily = queueFamilies[i];
				VkBool32 surfaceSupported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupported);
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && surfaceSupported)
				{
					graphicsQueueFamilyIndex = i;
					break;
				}
			}
			if (graphicsQueueFamilyIndex >= queueFamilyCount) continue;

			// try to find separate transfer queue family
			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				const auto& queueFamily = queueFamilies[i];
				if (i != graphicsQueueFamilyIndex && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					transferQueueFamilyIndex = i;
					break;
				}
			}

			uint32_t transferQueueIndex = 0;
			// if no separate queue family found, take graphics family but with different queue if possible
			if (transferQueueFamilyIndex >= queueFamilyCount)
			{
				transferQueueFamilyIndex = graphicsQueueFamilyIndex;
				if (queueFamilies[transferQueueFamilyIndex].queueCount > 1)
					transferQueueIndex = 1;
				else continue;
			}

			const auto priorities = std::vector{ 0.0f, 0.0f };
			VkDeviceQueueCreateInfo queueCreateInfos[2]{};
			queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[0].queueFamilyIndex = graphicsQueueFamilyIndex;
			queueCreateInfos[0].queueCount = 1 + transferQueueFamilyIndex; // 1 if separate families else 2
			queueCreateInfos[0].pQueuePriorities = priorities.data();
			// define 2nd struct, may simply be ignored later
			queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[1].queueFamilyIndex = transferQueueFamilyIndex;
			queueCreateInfos[1].queueCount = 1;
			queueCreateInfos[1].pQueuePriorities = priorities.data();

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = 2 - transferQueueFamilyIndex; // same trick as above
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(useValidation); // see VulkanInstance.h
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data(); // see VulkanInstance.h
			deviceCreateInfo.enabledExtensionCount = 1;
			deviceCreateInfo.ppEnabledExtensionNames = &swapchainEXT;
			deviceCreateInfo.pEnabledFeatures = nullptr;

			VkDevice device;
			auto res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
			PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create logical device!");

			return { physicalDevice, device };
		}

		PR_CORE_ASSERT(false, "No suitable device found!");
		return {};
	}
}