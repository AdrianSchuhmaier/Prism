#pragma once

#include "Resource.h"

namespace Prism {

	/**
	 * Static class abstracting individual resources into handles
	 * Especially important to not expose dependency to Vulkan
	 * 
	 * Used for loading and managing resource life-time
	 * May also (in the future) reorder data according to access patterns
	 */
	class ResourceManager {
	public:
		static void Init();
		static void Shutdown();

		template<typename T>
		static T* Get(ResourceHandle);

	private:
	};
}