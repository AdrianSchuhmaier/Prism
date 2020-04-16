#pragma once

#include "ResourceManager.h"
#include <memory>
#include <string>

namespace Prism {
	using ResourceHandle = std::string;

	template<typename T>
	class Resource {
	public:
		// implicit conversion to represented type
		constexpr operator T* () const { return ResourceManager::Get<T>(handle); }
		constexpr bool valid() const { return handle != ""; }

		template<typename... Args>
		static Resource<T> Create(const ResourceHandle& name, Args&&... args) { return ResourceManager::Create<T>(name, args...); }

		Resource(const ResourceHandle& handle) : handle(handle) {};
		ResourceHandle handle = "";

	public:
		Resource(const Resource& other) : handle(other.handle) { ResourceManager::Get<T>(handle); }
		Resource& operator=(const Resource& rhs) { return std::move(Resource{ ResourceManager::Get<T>(rhs.handle) }); }
		Resource(Resource&& other) noexcept : handle(std::move(other.handle)) { }
		Resource& operator=(Resource&& rhs) noexcept { if (this != &rhs) handle = std::move(handle); return *this; };
		~Resource() { if (valid()) ResourceManager::Remove<T>(handle); };
	};

	// forward-declarations for unexposed classes
	template<typename T> struct resource_t;

	class VulkanPipeline;
	struct pipeline_t {}; using Shader = VulkanPipeline;
	template<> struct resource_t<VulkanPipeline> { typedef pipeline_t type; };

	struct VulkanSwapchain;
	struct swapchain_t {}; using Swapchain = VulkanSwapchain;
	template<> struct resource_t<Swapchain> { typedef swapchain_t type; };
}