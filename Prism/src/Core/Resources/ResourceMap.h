#pragma once

#include "Resource.h"

#include <unordered_map>
#include <memory>
#include <atomic>

namespace Prism {

	/**
	 * Unordered map of shared_ptrs, managing the given resource
	 */
	template<typename R>
	class ResourceMap {
	public:

		/**
		 * Constructs a resource with the given parameters
		 * and stores a shared_ptr in the ResourceMap
		 *
		 * @returns the handle and a shared_ptr of the created resource
		 */
		template<typename... Args>
		ResourceHandle CreateResource(Args&&... args)
		{
			uint32_t handle = m_NextHandle.load();
			while (!m_NextHandle.compare_exchange_strong(handle, handle + 1));

			m_Map[handle] = std::make_shared<R>(std::forward<Args>(args)...);
			return handle;
		}

		/**
		 * @returns a shared_ptr to the resource with the given handle
		 */
		std::shared_ptr<R> GetResource(ResourceHandle handle)
		{
			if (handle < m_NextHandle)
				if (const auto& sp = m_Map.at(handle))
					return sp;
				else PR_CORE_WARN("Requested resource no longer exists!");
			else PR_CORE_WARN("No resource with this id ever existed!");

			return nullptr;
		}

		/**
		 * Removes a resource from the map.
		 *
		 * All existing shared_ptrs to the resource stay valid,
		 * but ResourceMap no longer keeps the resource alive.
		 *
		 * @returns if this call destroyed the resource
		 */
		bool RemoveResource(ResourceHandle handle)
		{
			if (handle < m_NextHandle)
				if (const auto& sp = m_Map.at(handle))
				{
					if (sp.use_count() > 1)
						PR_CORE_WARN("Resource still referenced, will be removed later");
					m_Map[handle] = nullptr;
				}
				else PR_CORE_WARN("Resource was already removed!");
			else PR_CORE_WARN("No resource with this id ever existed!");

			return false;
		}

	private:
		std::atomic<ResourceHandle> m_NextHandle = 0;
		std::unordered_map<ResourceHandle, std::shared_ptr<R>> m_Map{};
	};
}