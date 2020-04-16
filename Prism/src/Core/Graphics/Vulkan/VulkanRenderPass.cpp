#include "VulkanRenderPass.h"
#include "VulkanContext.h"

#include <array>

namespace Prism {

	VulkanRenderPass::VulkanRenderPass(VulkanContext* context, const RenderPassBP& blueprint)
		: m_Context(context)
	{
		std::vector<VkSubpassDescription> subPasses(blueprint.subpassBlueprints.size());
		for (uint32_t i = 0; i < blueprint.subpassBlueprints.size(); ++i)
		{
			const auto& subPassBlueprint = blueprint.subpassBlueprints[i];
			subPasses[i].flags = {};
			subPasses[i].inputAttachmentCount = static_cast<uint32_t>(subPassBlueprint.inputAttachments.size());
			subPasses[i].colorAttachmentCount = static_cast<uint32_t>(subPassBlueprint.colorAttachments.size());
			subPasses[i].preserveAttachmentCount = static_cast<uint32_t>(subPassBlueprint.preserveAttachments.size());

			subPasses[i].pInputAttachments = subPassBlueprint.inputAttachments.data();
			subPasses[i].pColorAttachments = subPassBlueprint.colorAttachments.data();
			subPasses[i].pDepthStencilAttachment = subPassBlueprint.depthStencilAttachment;
			subPasses[i].pResolveAttachments = subPassBlueprint.resolveAttachments.data();
			subPasses[i].pPreserveAttachments = subPassBlueprint.preserveAttachments.data();

			subPasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		}

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(blueprint.attachmentDescriptions.size());
		createInfo.pAttachments = blueprint.attachmentDescriptions.data();
		createInfo.subpassCount = static_cast<uint32_t>(subPasses.size());
		createInfo.pSubpasses = subPasses.data();
		createInfo.dependencyCount = static_cast<uint32_t>(blueprint.subpassDependencies.size());
		createInfo.pDependencies = blueprint.subpassDependencies.data();

		auto res = vkCreateRenderPass(context->GetDevice(), &createInfo, nullptr, &m_RenderPass);
		PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create RenderPass");

		SetClearValue({ { 1.0f, 0.0f, 1.0f, 1.0f } });
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		vkDestroyRenderPass(m_Context->GetDevice(), m_RenderPass, nullptr);
	}

	//void RenderPass::Begin(
	//	VkCommandBuffer cmd,
	//	VkFramebuffer framebuffer,
	//	VkSubpassContents nextSubpassType,
	//	std::optional<VkRect2D> renderArea) const
	//{
	//	VkRenderPassBeginInfo renderPassInfo(
	//		m_RenderPass, framebuffer,
	//		renderArea.value_or(VkRect2D({ 0, 0 }, Context::GetSwapchain().extent)),
	//		1, &m_ClearValue);
	//	cmd.beginRenderPass(renderPassInfo, nextSubpassType);
	//}
	//
	//void RenderPass::NextSubPass(VkCommandBuffer cmd, VkSubpassContents nextSubpassType) const
	//{
	//	// TODO: check for validity
	//	cmd.nextSubpass(nextSubpassType);
	//}
	//
	//void RenderPass::End(VkCommandBuffer cmd) const
	//{
	//	cmd.endRenderPass();
	//}
}