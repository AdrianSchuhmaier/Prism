#pragma once

#include "Util/Log/Log.h"

#include <vulkan/vulkan.h>

namespace Prism {

#ifdef PR_DEBUG
	constexpr bool useValidation = true;
	const auto validationLayers = std::vector{ "VK_LAYER_LUNARG_standard_validation" };
#else
	constexpr bool useValidation = false;
	const auto validationLayers = std::vector{ };
#endif

	/**
	 * Static class representing the (application-wide) instance of Vulkan
	 */
	class VulkanInstance {
	public:
		static void Init();
		static void Shutdown();

		static VkInstance Get()
		{
			PR_CORE_ASSERT(s_Initialized, "No VulkanInstance initialized!");
			return s_Instance;
		}

	private:
		static bool s_Initialized;
		static VkInstance s_Instance;
	};
}