#pragma once

#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace Prism {

	struct Component
	{
		virtual ~Component() = default;
	};

	/**
	 * Non-owning datastructure of Components (used in Entity.h)
	 *
	 *
	 */
	class ComponentsMap {
	public:
		template<typename T, typename ...Args>
		constexpr T* Create(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "Component classes must be derivatives of Component.");
			T* component = new T(std::forward<Args>(args)...);
			Register(component);
			components.insert(std::pair(&typeid(T), static_cast<Component*>(component)));
			return component;
		}

		template<typename T>
		constexpr void Add(T* component)
		{
			static_assert(std::is_base_of<Component, T>::value, "Component classes must be derivatives of Component.");
			components.insert(std::pair(&typeid(T), static_cast<Component*>(component)));
			return component;
		}

		template<typename T>
		constexpr T* Get()
		{
			const auto itr = components.find(&typeid(T));
			PR_CORE_ASSERT(itr != components.end(), "Component classes must be derivatives of Component.");
			return dynamic_cast<T*>(itr->second);
		}

	private:
		std::unordered_multimap<const std::type_info*, Component*> components;
	};
}