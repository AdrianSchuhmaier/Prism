#pragma once

#include <vulkan/vulkan.h>

namespace Prism {

	/**
	 * Static class representing the (application-wide) instance of Vulkan
	 */
	class VulkanInstance {
	public:
		static void Init();
		static void Shutdown();

		static VkInstance Get() { return s_Instance; }

	private:
		static bool s_Initialized;
		static VkInstance s_Instance;
	};
}