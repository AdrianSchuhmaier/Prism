#pragma once

#include "Resource.h"

#include "Util/Log/Log.h"

#include <unordered_map>
#include <memory>
#include <atomic>

namespace Prism {

	/**
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
		ResourceHandle CreateResource(const ResourceHandle& handle, Args&&... args)
		{
			uint32_t initialRefCount = 0;

			// if resource already exists
			auto itr = m_Map.find(handle);
			if (itr != m_Map.end() && itr->second.second.load() > 0)
			{ // replacing unreferenced existing resource is considered safe here
				PR_CORE_CRITICAL("Unsafe replacement of existing resource {0}!", handle);
				initialRefCount = itr->second.second.load();
			}

			// create resource
			m_Map.emplace(handle, std::pair{
				std::make_unique<R>(std::forward<Args>(args)...),
				initialRefCount + 1
				});

			return handle;
		}

		ResourceHandle GetResource(const ResourceHandle& handle)
		{
			// check if resource available
			auto itr = m_Map.find(handle);
			if (itr == m_Map.end())
			{
				PR_CORE_WARN("Resource {0} not found, invalid resource returned", handle);
				return { "" };
			}
			itr->second.second++; // increment refCount
			return handle;
		}

		R* GetRaw(const ResourceHandle& handle)
		{
			auto itr = m_Map.find(handle);
			if (itr == m_Map.end())
			{
				PR_CORE_WARN("Resource {0} not found, nullptr returned", handle);
				return nullptr;
			}

			return itr->second.first.get();
		}

		void NotifyCopy(const ResourceHandle& handle)
		{
			auto itr = m_Map.find(handle);
			if (itr != m_Map.end())
				itr->second.second++; // increment refCount
			else
				PR_CORE_WARN("Resource {0} not found, notifyCopy is pointless", handle);
		}

		/**
		 * Removes a resource from the map.
		 *
		 * All existing shared_ptrs to the resource stay valid,
		 * but ResourceMap no longer keeps the resource alive.
		 *
		 * @returns if this call destroyed the resource
		 */
		bool RemoveResource(const ResourceHandle& handle)
		{
			// check if resource available
			auto itr = m_Map.find(handle);
			if (itr == m_Map.end())
			{
				PR_CORE_ASSERT(false, "Trying to remove non-existent resource {0}", handle);
				return false;
			}
			else if (itr->second.second.load() < 1)
			{
				PR_CORE_ASSERT(false, "Resource {0} should have no references");
				return false;
			}

			itr->second.second--; // decrement refCount
			if (itr->second.second.load() == 1)
				PR_CORE_WARN("No resource destruction implemented");
			return false;
		}

		void Clear()
		{
			m_Map.clear();
		}

	private:
		std::unordered_map<ResourceHandle, std::pair<std::unique_ptr<R>, std::atomic<uint32_t>>> m_Map{};
	};
}