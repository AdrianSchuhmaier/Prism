#pragma once

#include "Entity.h"
#include "Systems/System.h"

#include "Util/Log/Log.h"

#include <unordered_map>
#include <vector>
#include <atomic>
#include <typeinfo>
#include <typeindex>

namespace Prism {

	/**
	 * Context for all game objects and systems
	 */
	class World {
	public:
		Entity* GetEntity(EntityID id);


		// owning hash-map for systems
		SystemsMap systems;

		// owning vector of components
		std::vector<std::unique_ptr<Component>> components;

		// non-owning unorderedMap for entities
		std::unordered_map<EntityID, Entity*> m_Entities{};


	private:
		friend class Entity;
		std::atomic<EntityID> m_GeneratorID{ 1 };

		void DestroyComponents(Entity* entity) {}
	};
}