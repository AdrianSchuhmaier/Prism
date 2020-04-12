#include "ResourceManager.h"

namespace Prism {
	
	void ResourceManager::Init()
	{
	}

	void ResourceManager::Shutdown()
	{
	}
	ResourceMap<VertexBuffer> ResourceManager::m_VertexBuffers{};
}