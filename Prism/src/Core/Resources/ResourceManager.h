#pragma once

#include "Resource.h"

namespace Prism {

	class Renderer;

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


		template<typename T, typename... Args>
		static Resource<T> Create(const ResourceHandle& name, Args&&... args)
		{
			typedef typename resource_t<T>::type tag;
			return Resource<T>{ Create(tag(), name, args...) };
		}

		template<typename T>
		static Resource<T> Get(const ResourceHandle& name)
		{
			typedef typename resource_t<T>::type tag;
			return Resource<T>{ Get(tag(), name) };
		}

		template<typename T>
		static void Remove(const ResourceHandle& name)
		{
			typedef typename resource_t<T>::type tag;
			Remove(tag(), name);
		}

	private:
		template<typename T> friend class Resource;

		template<typename T>
		static void NotifyCopy(const ResourceHandle& name)
		{
			typedef typename resource_t<T>::type tag;
			NotifyCopy(tag(), name);
		}

		template<typename T>
		static T* GetRaw(const ResourceHandle& name)
		{
			typedef typename resource_t<T>::type tag;
			return GetRaw(tag(), name);
		}

	private:
		
		/** 
		 * For any resource managed, these methods must be implemented.
		 * 
		 * 
		 * static ResourceHandle Create(uint32_t_t, const ResourceHandle& handle, ...);
		 * static ResourceHandle Get(uint32_t_t, const ResourceHandle& handle);
		 * static void NotifyCopy(uint32_t_t, const ResourceHandle& handle);
		 * static void Remove(uint32_t_t, const ResourceHandle& handle);
		 */

		static ResourceHandle Create(pipeline_t, const ResourceHandle& handle, const std::string& filepath, Renderer*);
		static ResourceHandle Get(pipeline_t, const ResourceHandle& handle);
		static VulkanPipeline* GetRaw(pipeline_t, const ResourceHandle& handle);
		static void NotifyCopy(pipeline_t, const ResourceHandle& handle);
		static void Remove(pipeline_t, const ResourceHandle& handle);

		//static ResourceHandle Create(swapchain_t, const ResourceHandle& handle);
		//static ResourceHandle Get(swapchain_t, const ResourceHandle& handle);
		//static void NotifyCopy(swapchain_t, const ResourceHandle& handle);
		//static void Remove(swapchain_t, const ResourceHandle& handle);
	};
}