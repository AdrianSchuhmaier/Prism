#pragma once

#include "ResourceMap.h"

#include "Util/Log/Log.h"

#include <unordered_map>
#include <memory>
#include <atomic>

namespace Prism {

	using VertexBuffer = uint32_t;

	class ResourceManager {
	public:


		static void Init();
		static void Shutdown();
	private:
		static ResourceMap<VertexBuffer> m_VertexBuffers;
	};
}