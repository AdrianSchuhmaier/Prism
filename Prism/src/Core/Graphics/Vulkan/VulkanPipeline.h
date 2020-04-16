#pragma once

#include "VulkanContext.h"

#include "ShaderUtil.h"

#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Prism {

	class VulkanPipeline {
	public:

		VulkanPipeline(VulkanContext* context, const ShaderBinary& binary);
		~VulkanPipeline();

	private:
		void setShaders(const ShaderBinary& binary);
		void setVertexInput();
		void setRasterizationState(VkCullModeFlags cullMode, VkFrontFace frontFace, VkPolygonMode polygonMode);
		void setMultisample(VkSampleCountFlagBits samples);

		struct ColorBlend { VkBlendFactor src, dst; VkBlendOp op; };
		void setColorBlend(const ColorBlend& color, const ColorBlend& alpha);

		void setLayout();

	private:
		VulkanContext* m_Context;
		VkPipeline m_Pipeline;

		VkPipelineLayout m_Layout;
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo m_ColorBlendCreateInfo;
		VkPipelineRasterizationStateCreateInfo m_RasterizationCreateInfo;
		VkPipelineMultisampleStateCreateInfo m_MultisampleCreateInfo;
		std::vector<VkDynamicState> m_DynamicStates;

		VkPipelineVertexInputStateCreateInfo m_VertexInputCreateInfo;
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyCreateInfo;

		struct ShaderModule {
			VkShaderModule module;
			VkPipelineShaderStageCreateInfo info;
		};
		std::unordered_map<ShaderType, ShaderModule> m_ShaderModules{};
	};
}