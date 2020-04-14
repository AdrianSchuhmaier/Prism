#pragma once

#include "Components/Component.h"

#include "Util/Log/Log.h"

#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace Prism {

	using EntityID = uint32_t;

	class Entity
	{
	public:
		EntityID id;

		Entity() { PR_CORE_WARN("Entity constructed"); };
		~Entity() {};

	private:
		// Wraps addedComponent (allocated with new) in a unique_ptr in Application::world
		void Register(Component* addedComponent);

	public:
		ComponentsMap components;
	};
}