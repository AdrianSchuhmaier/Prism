#include "ResourceManager.h"
#include "ResourceMap.h"

#include "Core/Graphics/Vulkan/VulkanPipeline.h"
#include "Core/Graphics/Renderer.h"
#include "Core/Graphics/Vulkan/VulkanRenderAPI.h"

#include "Core/Graphics/Vulkan/VulkanSwapchain.h"


namespace Prism {

	ResourceMap<uint32_t> g_UIntMap{};
	ResourceMap<VulkanSwapchain> g_SwapchainMap{};
	ResourceMap<VulkanPipeline> g_Pipelines{};

	void ResourceManager::Init()
	{
	}

	void ResourceManager::Shutdown()
	{
		g_Pipelines.Clear();
	}



	ResourceHandle ResourceManager::Create(pipeline_t, const ResourceHandle& handle,
		const std::string& filepath, Renderer* renderer)
	{
		auto binary = ShaderUtil::Load(filepath);
		if (!binary.has_value())
		{
			PR_CORE_WARN("Unable to load shader, returning invalid resource handle.");
			return "";
		}

		return g_Pipelines.CreateResource(handle, renderer-> m_Renderer->GetContext(), binary.value());
	}
	ResourceHandle ResourceManager::Get(pipeline_t, const ResourceHandle& handle) { return g_Pipelines.GetResource(handle); }
	VulkanPipeline* ResourceManager::GetRaw(pipeline_t, const ResourceHandle& handle) { return g_Pipelines.GetRaw(handle); }
	void ResourceManager::NotifyCopy(pipeline_t, const ResourceHandle& handle) { g_Pipelines.NotifyCopy(handle); }
	void ResourceManager::Remove(pipeline_t, const ResourceHandle& handle) { g_Pipelines.RemoveResource(handle); }

}