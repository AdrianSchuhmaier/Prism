#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <optional>

namespace Prism {

	class VulkanContext;

	class VulkanRenderPass
	{
	public:
		struct SubpassBP
		{
			std::vector<VkAttachmentReference> inputAttachments = {};
			std::vector<VkAttachmentReference> colorAttachments = {};
			std::vector<VkAttachmentReference> resolveAttachments = {};
			VkAttachmentReference* depthStencilAttachment = nullptr;
			std::vector<uint32_t> preserveAttachments = {};
		};

		struct RenderPassBP
		{
			std::vector<VkAttachmentDescription> attachmentDescriptions = {};
			std::vector<SubpassBP> subpassBlueprints = {};
			std::vector<VkSubpassDependency> subpassDependencies = {};
		};

		VulkanRenderPass(VulkanContext* context, const RenderPassBP& blueprint);
		~VulkanRenderPass();

		//void Begin(VkCommandBuffer cmd, VkFramebuffer framebuffer, VkSubpassContents nextSubpassType,
		//	std::optional<VkRect2D> renderArea = std::nullopt) const;
		//void NextSubPass(VkCommandBuffer cmd, VkSubpassContents nextSubpassType) const;
		//void End(VkCommandBuffer cmd) const;

		void SetClearValue(VkClearValue clearValue) { m_ClearValue = clearValue; }
		VkRenderPass GetHandle() const { return m_RenderPass; }

	private:
		VulkanContext* m_Context;
		VkRenderPass m_RenderPass;
		VkClearValue m_ClearValue;
	};
}