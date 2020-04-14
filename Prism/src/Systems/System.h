#pragma once

#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace Prism {

	class System {
	public:
		virtual ~System() = default;

		friend class SystemsMap;
	protected:
		System() = default;
	};

	/**
	 * Owning datastructure for Systems (used in World.h)
	 * 
	 * Manages lifetime of the systems and only returns raw pointers
	 */
	class SystemsMap {
	public:
		template<typename T, typename ...Args>
		constexpr T* Create(Args&&... args)
		{
			static_assert(std::is_base_of<System, T>::value, "System classes must be derivatives of System.");
			T* system = new T(std::forward<Args>(args)...);
			systems.insert(std::pair(&typeid(T), std::unique_ptr<System>(system)));
			return system;
		}

		template<typename T>
		constexpr T* Get()
		{
			const auto itr = systems.find(&typeid(T));
			if (itr != systems.end())
				return static_cast<T*>(itr->second.get());
			PR_CORE_CRITICAL("System not found!");
			return nullptr;
		}

	private:
		std::unordered_map<const std::type_info*, std::unique_ptr<System>> systems;
	};
}