#include "VulkanInstance.h"

#include "Util/Log/Log.h"

#include <GLFW/glfw3.h>
#include <vector>

namespace Prism {

	bool VulkanInstance::s_Initialized = false;
	VkInstance VulkanInstance::s_Instance{};

	VkDebugUtilsMessengerEXT debugMessenger;
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		PR_CORE_ERROR(pCallbackData->pMessage);

		return VK_FALSE;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	// Helper functions, defined below
	std::vector<const char*> getRequiredExtensions();
	bool checkExtensionsAvailable(const std::vector<const char*>& extensions);
	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);



	void VulkanInstance::Init()
	{
		PR_CORE_ASSERT(!s_Initialized, "VulkanInstance must be initialized only once!");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Prism";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Prism";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		auto requiredExtensions = getRequiredExtensions();
		PR_CORE_ASSERT(checkExtensionsAvailable(requiredExtensions), "VulkanExtensions missing!");


		if (useValidation) PR_CORE_ASSERT(checkValidationLayerSupport(validationLayers), "ValidationLayer missing!");

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (useValidation) {
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = debugCallback;

			createInfo.pNext = &debugCreateInfo;
		}

		auto result = vkCreateInstance(&createInfo, nullptr, &s_Instance);
		PR_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");
		PR_CORE_INFO("Vulkan instance created");

		if (useValidation) {
			auto res = createDebugUtilsMessengerEXT(s_Instance, &debugCreateInfo, nullptr, &debugMessenger);
			PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to setup Vulkan debugMessenger");
		}

		s_Initialized = true;
	}

	void VulkanInstance::Shutdown()
	{
		if (useValidation) {
			destroyDebugUtilsMessengerEXT(s_Instance, debugMessenger, nullptr);
		}
		vkDestroyInstance(s_Instance, nullptr);

		s_Initialized = false;
	}



	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (useValidation)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	bool checkExtensionsAvailable(const std::vector<const char*>& requiredExtensions)
	{
		uint32_t availableExtensionsCount;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

		// count the number of required extensions that are available
		uint32_t confirmedCount = 0;
		for (const auto& required : requiredExtensions)
			for (const auto& available : availableExtensions)
				if (strcmp(available.extensionName, required) == 0)
				{
					++confirmedCount;
					break;
				}

		return confirmedCount == requiredExtensions.size();
	}

	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// count the number of required layers that are available
		uint32_t confirmedCount = 0;
		for (const auto& required : validationLayers)
			for (const auto& available : availableLayers)
				if (strcmp(available.layerName, required) == 0)
				{
					++confirmedCount;
					break;
				}

		return confirmedCount == validationLayers.size();
	}
}