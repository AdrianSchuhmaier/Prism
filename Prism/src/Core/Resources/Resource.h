#pragma once

#include "ResourceManager.h"
#include <memory>

namespace Prism {
	using ResourceHandle = uint32_t;

	template<typename T>
	struct Resource {
		constexpr operator T* () const { return ResourceManager::Get<T>(handle); }
		ResourceHandle handle;

	private:
		friend class ResourceManager;
		Resource();
	};

	// forward-declarations for unexposed classes
}