#include "World.h"

namespace Prism {

	Entity* World::GetEntity(EntityID id)
	{
		auto itr = m_Entities.find(id);
		if (itr != m_Entities.end())
			return itr->second;
		PR_CORE_WARN("Could not find Entity with id {0}", id);
		return nullptr;
	}
}