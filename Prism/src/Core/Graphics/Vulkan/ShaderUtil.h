#pragma once

#include "Util/Log/Log.h"
#include "Util/FileReader/StringReader.h"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <streambuf>

namespace Prism {

	// ordering matters here (same as in shaderc.hpp)!
	enum class ShaderType {
		Vertex,
		Fragment,
		Compute,
		Geometry,
		TesselationControl,
		TesselationEvaluation
	};

	VkShaderStageFlagBits shaderTypeToVulkanStageFlag(const ShaderType& type);

	using ShaderCode = std::unordered_map<ShaderType, std::string>;
	using ShaderBinary = std::unordered_map<ShaderType, std::vector<uint32_t>>;

	class ShaderUtil {
	public:

		static std::optional<ShaderBinary> Load(const std::string& filepath);
		static std::optional<ShaderBinary> Compile(const ShaderCode& code, const char* name);
		static std::optional<ShaderCode> ReadFile(const std::string& filepath);
	};
}