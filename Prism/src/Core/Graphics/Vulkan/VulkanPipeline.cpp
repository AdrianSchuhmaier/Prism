#include "VulkanPipeline.h"

namespace Prism {

	VulkanPipeline::VulkanPipeline(VulkanContext* context, const ShaderBinary& binary)
		: m_Context(context)
		, m_DynamicStates({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH })
	{
		setShaders(binary);
		setVertexInput();
		setRasterizationState(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_POLYGON_MODE_FILL);
		setMultisample(VK_SAMPLE_COUNT_1_BIT);
		setColorBlend(
			{ VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD },
			{ VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD });
		setLayout();

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)context->GetSwapchain()->extent.width;
		viewport.height = (float)context->GetSwapchain()->extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.extent = context->GetSwapchain()->extent;
		scissor.offset = { 0, 0 };

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
		dynamicState.pDynamicStates = m_DynamicStates.data();

		std::vector<VkPipelineShaderStageCreateInfo> stages;
		stages.reserve(5);

		// vertex + fragment shader must be there
		stages.emplace_back(m_ShaderModules.at(ShaderType::Vertex).info);
		stages.emplace_back(m_ShaderModules.at(ShaderType::Fragment).info);
		// add other shaders (if found)
		if (m_ShaderModules.find(ShaderType::Geometry) != m_ShaderModules.end())
			stages.emplace_back(m_ShaderModules.at(ShaderType::Geometry).info);
		if (m_ShaderModules.find(ShaderType::TesselationControl) != m_ShaderModules.end())
			stages.emplace_back(m_ShaderModules.at(ShaderType::TesselationControl).info);
		if (m_ShaderModules.find(ShaderType::TesselationEvaluation) != m_ShaderModules.end())
			stages.emplace_back(m_ShaderModules.at(ShaderType::TesselationEvaluation).info);


		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = static_cast<uint32_t>(stages.size());
		createInfo.pStages = stages.data();
		createInfo.pVertexInputState = &m_VertexInputCreateInfo;
		createInfo.pInputAssemblyState = &m_InputAssemblyCreateInfo;
		createInfo.pTessellationState = nullptr;
		createInfo.pViewportState = &viewportState;
		createInfo.pRasterizationState = &m_RasterizationCreateInfo;
		createInfo.pMultisampleState = &m_MultisampleCreateInfo;
		createInfo.pDepthStencilState = nullptr;
		createInfo.pColorBlendState = &m_ColorBlendCreateInfo;
		createInfo.pDynamicState = &dynamicState;
		createInfo.layout = m_Layout;
		createInfo.renderPass = context->GetDefaultRenderPass();
		createInfo.subpass = 0;
		createInfo.basePipelineHandle = nullptr;
		createInfo.basePipelineIndex = -1;

		auto res = vkCreateGraphicsPipelines(context->GetDevice(), nullptr, 1, &createInfo, nullptr, &m_Pipeline);

		PR_CORE_TRACE("Pipeline created");
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(m_Context->GetDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Context->GetDevice(), m_Layout, nullptr);
		for (const auto& shaderModule : m_ShaderModules)
			vkDestroyShaderModule(m_Context->GetDevice(), shaderModule.second.module, nullptr);
	}

	void VulkanPipeline::setShaders(const ShaderBinary& spv)
	{
		m_ShaderModules.clear();
		for (const auto& shader : spv)
		{
			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = sizeof(uint32_t) * shader.second.size();
			moduleCreateInfo.pCode = shader.second.data();

			auto res = vkCreateShaderModule(m_Context->GetDevice(), &moduleCreateInfo, nullptr, &m_ShaderModules[shader.first].module);
			PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create shader module");

			m_ShaderModules[shader.first].info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			m_ShaderModules[shader.first].info.stage = shaderTypeToVulkanStageFlag(shader.first);
			m_ShaderModules[shader.first].info.module = m_ShaderModules[shader.first].module;
			m_ShaderModules[shader.first].info.pName = "main";
		}
	}

	void VulkanPipeline::setVertexInput()
	{
		m_VertexInputCreateInfo = {};
		m_VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_VertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		m_VertexInputCreateInfo.pVertexBindingDescriptions = nullptr; // Optional
		m_VertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		m_VertexInputCreateInfo.pVertexAttributeDescriptions = nullptr; // Optional

		m_InputAssemblyCreateInfo = {};
		m_InputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_InputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_InputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;
	}

	void VulkanPipeline::setRasterizationState(
		VkCullModeFlags cullMode, VkFrontFace frontFace, VkPolygonMode polygonMode)
	{
		m_RasterizationCreateInfo = {};
		m_RasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_RasterizationCreateInfo.depthClampEnable = VK_FALSE;
		m_RasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		m_RasterizationCreateInfo.polygonMode = polygonMode;
		m_RasterizationCreateInfo.cullMode = cullMode;
		m_RasterizationCreateInfo.frontFace = frontFace;
		m_RasterizationCreateInfo.depthBiasEnable = VK_FALSE;
		m_RasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
		m_RasterizationCreateInfo.depthBiasClamp = 0.0f;
		m_RasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
		m_RasterizationCreateInfo.lineWidth = 1.0f;
	}

	void VulkanPipeline::setMultisample(VkSampleCountFlagBits samples)
	{
		m_MultisampleCreateInfo = {};
		m_MultisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_MultisampleCreateInfo.rasterizationSamples = samples;
		m_MultisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		m_MultisampleCreateInfo.minSampleShading = 1.0f;
		m_MultisampleCreateInfo.pSampleMask = nullptr;
		m_MultisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
		m_MultisampleCreateInfo.alphaToOneEnable = VK_FALSE;
	}

	void VulkanPipeline::setColorBlend(const ColorBlend& color, const ColorBlend& alpha)
	{
		m_ColorBlendAttachment = {};
		m_ColorBlendAttachment.blendEnable = VK_FALSE;
		m_ColorBlendAttachment.srcColorBlendFactor = color.src;
		m_ColorBlendAttachment.dstColorBlendFactor = color.dst;
		m_ColorBlendAttachment.colorBlendOp = color.op;
		m_ColorBlendAttachment.srcAlphaBlendFactor = alpha.src;
		m_ColorBlendAttachment.dstAlphaBlendFactor = alpha.dst;
		m_ColorBlendAttachment.alphaBlendOp = alpha.op;
		m_ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		m_ColorBlendCreateInfo = {};
		m_ColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_ColorBlendCreateInfo.logicOpEnable = VK_FALSE;
		m_ColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		m_ColorBlendCreateInfo.attachmentCount = 1;
		m_ColorBlendCreateInfo.pAttachments = &m_ColorBlendAttachment;
		m_ColorBlendCreateInfo.blendConstants[0] = 0.0f;
		m_ColorBlendCreateInfo.blendConstants[1] = 0.0f;
		m_ColorBlendCreateInfo.blendConstants[2] = 0.0f;
		m_ColorBlendCreateInfo.blendConstants[3] = 0.0f;
	}

	void VulkanPipeline::setLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		auto res = vkCreatePipelineLayout(m_Context->GetDevice(), &pipelineLayoutInfo, nullptr, &m_Layout);
		PR_CORE_ASSERT(res == VK_SUCCESS, "Failed to create pipeline layout");
	}
}